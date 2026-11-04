#include <nds.h>
#include <fat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "protocol.h"
#include "storage.h"
#include "wireless.h"
#include "mii_bridge.h"
#include "service_config.h"
#include "visitor.h"
#include "discovery.h"
#include "bark_mode.h"
#include "relay.h"
#include "plaza_roster.h"
#include "plaza_scene.h"
#include "plaza_motion.h"
#include "arrival_notice.h"
#include "puzzle_swap.h"

static PrintConsole top_screen, bottom_screen;
static char save_dir[SPDS_PATH_MAX];
static SpdsRecord myself;
static SpdsPollStats last_poll;
static int storage_ok;
static unsigned total;
static unsigned waiting_visitors;
static SpdsWireless radio;
static SpdsServiceConfig service_cfg;
static SpdsMii linked_mii;
static int mii_loaded;
static SpdsVisitorStats visitor_stats;
static SpdsMii visitors_on_screen[3];
static int visitor_count;
static SpdsPlazaRoster roster;
static SpdsPlazaScene visible_scene;
static int roster_open;
static int room_open;
static int mii_picker_open;
static unsigned mii_picker_slot;
static SpdsMii mii_picker_preview;
static int mii_picker_valid;
static unsigned room_selected;
static unsigned room_anim_frame;
static unsigned relay_send_index;
static unsigned relay_send_frames;
static int auto_radio;
static unsigned role_check_frames;
static SpdsBarkMode bark;
static int sleep_override;
static uint8_t outgoing[SPDS_RELAY_SIZE], incoming[SPDS_RELAY_SIZE];
static SpdsRelay relay;
static SpdsArrivalNotice recent_arrival;
static SpdsPuzzle puzzle;
static int puzzle_ready;
static int puzzle_open;
static int puzzle_detail;
static unsigned puzzle_save_errors;
static const char *radio_status(void) {
    if (radio.mode == SPDS_WIFI_OFF) return "Off";
    if (radio.mode == SPDS_WIFI_MONITOR) return "3DS probe listen";
    if (radio.phase == SPDS_PHASE_START) return "Starting";
    if (radio.phase == SPDS_PHASE_SCANNING) return "Scanning";
    if (radio.phase == SPDS_PHASE_CONNECTING) return "Connecting";
    if (radio.phase == SPDS_PHASE_LINKED)
        return radio.mode == SPDS_WIFI_HOST ? "Host" : "Joined";
    return "Radio error";
}

/* Import a previously saved Mii Maker DS profile without ever writing to
 * the original .sav. Only 7-bit nickname travels on SPDS v1 radio packets;
 * full Mii appearance remains in local mii.mds until a verified v2 exists.
 */
static int load_mii_from_sd(void) {
    if (!storage_ok || !spds_service_read(save_dir,&service_cfg)) return 0;
    auto_radio=service_cfg.enabled && service_cfg.radio_enabled;
    int lid_permission=auto_radio && service_cfg.lid_search_enabled;
    if(lid_permission && !sleep_override) { disableSleep(); sleep_override=1; }
    else if(!lid_permission && sleep_override) { enableSleep(); sleep_override=0; }
    spds_bark_init(&bark,lid_permission);
    /* Preserve the user's explicit Plaza selection across restarts. */
    SpdsMii saved_choice;
    if(spds_mii_cache_read(&saved_choice,save_dir) &&
       saved_choice.from_gallery) {
        linked_mii=saved_choice;
    } else {
        int imported=spds_service_load_mii(save_dir,&service_cfg,&linked_mii);
        if(imported<=0) return imported;
    }
    mii_loaded=1;
    if(linked_mii.features[47]) { /* Export appearance only when sharing allowed. */
        spds_mii_radio_name(myself.name,&linked_mii);
        if(spds_storage_save_profile(save_dir,&myself)!=0) return -1;
        if(!spds_relay_encode(outgoing,&myself,SPDS_RELAY_MAX_HOPS,(uint32_t)time(NULL))) return -1;
        if(!spds_visitor_export(save_dir,&myself,&linked_mii)) return -1;
    } else {
        /* Never share a Mii with their sharing bit disabled. Remove any
         * stale full-appearance export after the user edits this policy. */
        char old[SPDS_PATH_MAX];
        int n=snprintf(old,sizeof old,"%s/outbox/visitor.spv",save_dir);
        if(n>=0 && n<(int)sizeof old) remove(old);
        /* Keep the locally selected Plaza identity even if copying is disabled. */
        spds_mii_radio_name(myself.name,&linked_mii);
        if(!spds_relay_encode(outgoing,&myself,SPDS_RELAY_MAX_HOPS,(uint32_t)time(NULL)) ||
           spds_storage_save_profile(save_dir,&myself)!=0) return -1;
    }
    return 1;
}
static void refresh_roster(void) {
    if (!storage_ok) {
        visitor_count=0;
        spds_plaza_roster_set_count(&roster,0);
        return;
    }
    int total_visitors=spds_visitor_list(save_dir,NULL,0);
    if (total_visitors<0) total_visitors=0;
    spds_plaza_roster_set_count(&roster,(unsigned)total_visitors);
    if (!spds_plaza_scene_build(&roster,&visible_scene)) {
        visitor_count=0;
        return;
    }
    int page_total=spds_visitor_list_page(save_dir,
         visible_scene.first,visitors_on_screen,visible_scene.count);
    unsigned page_start=visible_scene.first;
    visitor_count=page_total>0 && (unsigned)page_total>page_start ?
                  (int)((unsigned)page_total-page_start) : 0;
    if(visitor_count>(int)SPDS_PLAZA_PAGE_SIZE)
        visitor_count=(int)SPDS_PLAZA_PAGE_SIZE;
}
static void save_puzzle_progress(void) {
    if(!storage_ok || !puzzle_ready) return;
    SpdsPuzzle next=puzzle;
    if(spds_puzzle_award_encounters(&next,total)) {
        if(spds_puzzle_save(save_dir,&next)==0) puzzle=next;
        else ++puzzle_save_errors;
    }
}
static int preview_mii_slot(void) {
    mii_picker_valid=0;
    if(!storage_ok || !spds_service_read(save_dir,&service_cfg) || !service_cfg.mii_source[0]) return 0;
    char path[SPDS_PATH_MAX];
    int absolute=strchr(service_cfg.mii_source,':')!=NULL || service_cfg.mii_source[0]=='/';
    int n=absolute?snprintf(path,sizeof path,"%s",service_cfg.mii_source):
        snprintf(path,sizeof path,"%s/%s",save_dir,service_cfg.mii_source);
    if(n<0 || n>=(int)sizeof path) return 0;
    mii_picker_valid=spds_mii_from_file_slot(&mii_picker_preview,path,mii_picker_slot);
    return mii_picker_valid;
}
static void draw(void) {
    consoleSelect(&top_screen);
    if(mii_picker_open) {
        printf("\x1b[2J\x1b[2;2HMII MAKER GALLERY");
        printf("\x1b[5;2HSlot %u / 100",mii_picker_slot+1u);
        printf("\x1b[7;2H%s",mii_picker_valid?mii_picker_preview.name:"Empty or unavailable");
        consoleSelect(&bottom_screen);
        printf("\x1b[2J\x1b[2;2HCHOOSE YOUR PLAZA MII");
        printf("\x1b[6;2HLEFT/RIGHT: Change slot");
        printf("\x1b[8;2HA: Choose this Mii");
        printf("\x1b[10;2HB: Cancel");
        printf("\x1b[13;2HSource: service.cfg");
        printf("\x1b[14;2Hmii_source= gallery save");
        return;
    }
    if(room_open) {
        printf("\x1b[2J\x1b[1;3HSTREETPASS PLAZA ROOM");
        printf("\x1b[3;2HVisitors: %d  Page %u/%u",visitor_count,
               spds_plaza_roster_page(&roster)+1u,spds_plaza_roster_pages(&roster));
        printf("\x1b[4;2H+----------------------------+");
        printf("\x1b[5;2H|                            |");
        printf("\x1b[12;2H+----------------------------+");
        printf("\x1b[13;2H  .  .  .  .  .  .  .  .");
        printf("\x1b[14;2H==============================");
        SpdsPlazaMotion motion;
        if(!spds_plaza_motion_sample(&visible_scene,room_selected,room_anim_frame,&motion))
            motion.count=0;
        for(unsigned i=0;i<motion.count && i<(unsigned)visitor_count;i++) {
            unsigned x=motion.actors[i].x;
            printf("\x1b[6;%uH%s",x,motion.actors[i].selected?"   v":"    ");
            printf("\x1b[7;%uH%s",x,motion.actors[i].selected?" [O] ":"  O  ");
            unsigned stride=motion.actors[i].walk_frame;
            printf("\x1b[8;%uH%s",x,stride?" \\|/ ":" /|\\\\ ");
            printf("\x1b[9;%uH%s",x,stride?"  /| ":" / \\\\ ");
            printf("\x1b[10;%uH%s",x,motion.actors[i].selected?" ===== ":"       ");
            printf("\x1b[11;%uH%.9s",x,visitors_on_screen[i].name);
        }
        if(!visitor_count) printf("\x1b[9;2HNo visitors in this room");
        printf("\x1b[16;2HPlaza stage / animated visitors");
        printf("\x1b[18;2HOriginal 3DS art not included");
        consoleSelect(&bottom_screen);
        printf("\x1b[2J\x1b[1;2HSELECT PLAZA VISITOR");
        for(int i=0;i<visitor_count && i<3;i++)
            printf("\x1b[%d;2H%c %-.24s",5+i*3,
                   i==(int)room_selected?'>':' ',visitors_on_screen[i].name);
        if(!visitor_count) printf("\x1b[7;2HNo visitors available");
        printf("\x1b[16;2HD-Pad: Select visitor");
        printf("\x1b[17;2HA: Open visitor details");
        printf("\x1b[19;2HLEFT/RIGHT: Room page");
        printf("\x1b[20;2HNo touch selection in room");
        printf("\x1b[21;2HB: Back to visitor roster");
        return;
    }
    printf("\x1b[2J\x1b[1;3HSTREETPASS MII PLAZA DS");
    printf("\x1b[4;2HIndependent homebrew prototype");
    if(recent_arrival.frames_left)
        printf("\x1b[5;2HNew DS visitor: %.20s",recent_arrival.name);
    printf("\x1b[6;2HVisitor: %-20s", myself.name);
    printf("\x1b[7;2HMaker Mii: %-20s",mii_loaded?linked_mii.name:"None linked");
    printf("\x1b[8;2HEncounters saved: %u", total);
    printf("\x1b[9;2HWaiting queue: %u/10",waiting_visitors);
    printf("\x1b[10;2HCached Mii visitors: %u",roster.count);
    printf("\x1b[10;2HLocal radio: %-14s", radio_status());
    printf("\x1b[11;2HAuto scan: %s",auto_radio?"Enabled":"Disabled");
    printf("\x1b[12;2HLid search: %-14s",
           sleep_override?"EXPERIMENTAL":"Off");
    printf("\x1b[13;2H3DS: no native CECD link");
    printf("\x1b[14;2HBackground: menu process only");
    printf("\x1b[15;2HRaw %u 3DS %u Plaza %u",radio.probe_frames,radio.probe_hits,radio.plaza_probe_hits);
    if(puzzle_ready) printf("\x1b[21;2HPuzzle pieces: %u/%u",
                           spds_puzzle_completed(&puzzle),SPDS_PUZZLE_PIECES);
    printf("\x1b[16;2H%s",roster_open?"Plaza roster (page view):":"Recent saved visitor Miis:");
    if(!visitor_count) printf("\x1b[18;2HNo full-Mii visitors yet");
    else for(int i=0;i<visitor_count && i<3;i++) {
        printf("\x1b[%d;2H%c %-20.20s",18+i,
               roster_open && (unsigned)i==roster.selected%SPDS_PLAZA_PAGE_SIZE ? '>' : ' ',
               visitors_on_screen[i].name);
    }
    consoleSelect(&bottom_screen);
    if(puzzle_open) {
        printf("\x1b[2J\x1b[1;2HPUZZLE SWAP - DS PREVIEW");
        if(!puzzle_ready) {
            printf("\x1b[5;2HSave data unavailable");
            printf("\x1b[18;2HB/X: Back");
            return;
        }
        printf("\x1b[3;2HCollected %u of %u pieces",
               spds_puzzle_completed(&puzzle),SPDS_PUZZLE_PIECES);
        for(unsigned i=0;i<SPDS_PUZZLE_PIECES;i++) {
            unsigned row=i/SPDS_PUZZLE_COLS;
            unsigned col=i%SPDS_PUZZLE_COLS;
            printf("\x1b[%u;%uH%c%c",5u+row*3u,4u+col*5u,
                   i==puzzle.selected?'>':' ',
                   (puzzle.pieces & (UINT32_C(1)<<i))?'#':'.');
        }
        if(puzzle_detail) printf("\x1b[15;2HPiece %u: %s",
          puzzle.selected+1u,
          (puzzle.pieces&(UINT32_C(1)<<puzzle.selected))?"Collected":"Missing");
        printf("\x1b[17;2H# owned / . missing");
        printf("\x1b[19;2HD-Pad/Touch: Select piece");
        printf("\x1b[20;2HA: Inspect  B/X: Back");
        printf("\x1b[21;2HPrototype: local DS rewards");
        printf("\x1b[22;2HNo original puzzle artwork");
        return;
    }
    if(roster_open) {
        printf("\x1b[2J\x1b[1;2HPLAZA VISITORS");
        printf("\x1b[3;2HStored %u   Page %u/%u",roster.count,
               roster.count?spds_plaza_roster_page(&roster)+1u:0u,
               spds_plaza_roster_pages(&roster));
        for(int i=0;i<visitor_count && i<3;i++) {
            printf("\x1b[%d;2H%c %-24.24s",5+2*i,
                   (unsigned)i==roster.selected%SPDS_PLAZA_PAGE_SIZE ? '>' : ' ',
                   visitors_on_screen[i].name);
        }
        if(!visitor_count) printf("\x1b[7;2HNo visitors saved yet");
        if(roster.detail_open && visitor_count) {
            unsigned slot=roster.selected%SPDS_PLAZA_PAGE_SIZE;
            if(slot<(unsigned)visitor_count) {
                printf("\x1b[12;2HSELECTED VISITOR");
                printf("\x1b[13;2HName: %-24.24s",visitors_on_screen[slot].name);
                printf("\x1b[14;2HMade by: %-22.22s",visitors_on_screen[slot].creator);
            }
        }
        printf("\x1b[17;2H< LEFT       RIGHT >");
        printf("\x1b[18;2HY / Touch: Puzzle Swap");
        printf("\x1b[19;2HUP/DOWN: Select visitor");
        printf("\x1b[20;2HA: Info  Touch: Select");
        printf("\x1b[21;2HB: Back  X: Close");
        printf("\x1b[22;2HSTART: Plaza room");
        return;
    }
    printf("\x1b[2J\x1b[1;2HSTREETPASS INBOX");
    printf("\x1b[4;2HStorage: %s", storage_ok ? "Ready" : "UNAVAILABLE");
    printf("\x1b[6;2HNew %u / duplicate %u",
            last_poll.accepted, last_poll.duplicates);
    printf("\x1b[8;2HWaiting queue: %u/10",waiting_visitors);
    printf("\x1b[9;2HInvalid: %u  I/O errors: %u",
            last_poll.invalid, last_poll.io_errors);
    printf("\x1b[10;2HPeer ACK: %u (unauthenticated)",radio.ack.confirmed);
    printf("\x1b[11;2HA: Export your profile");
    printf("\x1b[13;2HX: View Plaza visitors");
    printf("\x1b[14;2HY: Choose Plaza Mii");
    printf("\x1b[15;2HB: Exit to loader");
    printf("\x1b[16;2HL: Host    R: Join/Scan");
    printf("\x1b[17;2HSELECT: Off; SEL+R: Probe");
    printf("\x1b[18;2HRX: %u TX: %u", radio.received, radio.sent);
    printf("\x1b[19;1HSD file exchange is a test");
    printf("\x1b[20;1Htransport, NOT radio StreetPass.");
    printf("\x1b[21;1HPlaza Mii cache: %s",mii_loaded?"Ready":"Not linked");
    printf("\x1b[22;1HSTART: Refresh / save err %u",puzzle_save_errors);
}
static void poll_now(void) {
    if (storage_ok) {
        if (spds_storage_poll(save_dir, myself.sender_id,
                              (uint32_t)time(NULL), &last_poll) != 0)
            last_poll.io_errors++;
        SpdsVisitorStats imported;
        if (spds_visitor_poll(save_dir,myself.sender_id,
                              (uint32_t)time(NULL),&imported) == 0) {
            visitor_stats=imported;
            last_poll.accepted+=imported.accepted;
            last_poll.duplicates+=imported.duplicates;
            last_poll.invalid+=imported.invalid;
            last_poll.io_errors+=imported.io_errors;
        } else last_poll.io_errors++;
        if(spds_storage_total(save_dir, &total)==0) save_puzzle_progress();
        else ++last_poll.io_errors;
        if(spds_storage_queue_count(save_dir,&waiting_visitors)!=0) ++last_poll.io_errors;
        refresh_roster();
    }
}
int main(void) {
    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);
    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);
    consoleInit(&top_screen, 3, BgType_Text4bpp, BgSize_T_256x256,
                31, 0, true, true);
    consoleInit(&bottom_screen, 3, BgType_Text4bpp, BgSize_T_256x256,
                31, 0, false, true);
    spds_plaza_roster_init(&roster);
    memset(&myself, 0, sizeof(myself));
    strcpy(myself.name, "DS Traveler");
    const char *drive = NULL;
    if (fatInitDefault()) drive = fatGetDefaultDrive();
    if (drive && snprintf(save_dir, sizeof(save_dir),
                          "%sstreetpass-plaza", drive) <
                     (int)sizeof(save_dir) &&
        spds_storage_init(save_dir) == 0) {
        storage_ok = 1;
        if (spds_storage_load_profile(save_dir, &myself) != 0) {
            /* Local identity is random best-effort, not cryptographic. */
            unsigned seed = (unsigned)time(NULL) ^ (unsigned)REG_VCOUNT;
            srand(seed);
            myself.sender_id = ((uint64_t)(unsigned)rand() << 32) |
                               (uint64_t)(unsigned)rand();
            if (!myself.sender_id) myself.sender_id = 1;
            myself.timestamp = (uint32_t)time(NULL);
            myself.nonce = (uint32_t)rand();
            spds_storage_save_profile(save_dir, &myself);
        }
        if (spds_service_create(save_dir)) load_mii_from_sd();
        if(spds_puzzle_load(save_dir,&puzzle)>=0) puzzle_ready=1;
        else puzzle_ready=0; /* Never overwrite a corrupt existing save. */
        poll_now();
    }
    spds_relay_init(&relay,myself.sender_id,1);
    spds_relay_encode(outgoing,&myself,SPDS_RELAY_MAX_HOPS,(uint32_t)time(NULL));
    draw();
    unsigned frames = 0;
    for (;;) {
        swiWaitForVBlank();
        scanKeys();
        unsigned down = keysDown();
        if(room_open && (++room_anim_frame%12u)==0u) draw();
        if(mii_picker_open) {
            if(down & KEY_B) { mii_picker_open=0; draw(); continue; }
            if(down & (KEY_LEFT|KEY_RIGHT)) {
                mii_picker_slot=(mii_picker_slot+(down&KEY_LEFT?99u:1u))%100u;
                preview_mii_slot();draw();continue;
            }
            if((down & KEY_A) && mii_picker_valid) {
                linked_mii=mii_picker_preview;
                if(spds_mii_cache_write(&linked_mii,save_dir)) {
                    mii_loaded=1;
                    spds_mii_radio_name(myself.name,&linked_mii);
                    if(spds_storage_save_profile(save_dir,&myself)!=0) last_poll.io_errors++;
                    if(!spds_relay_encode(outgoing,&myself,SPDS_RELAY_MAX_HOPS,(uint32_t)time(NULL))) last_poll.io_errors++;
                    if(linked_mii.features[47]) {
                        if(!spds_visitor_export(save_dir,&myself,&linked_mii)) last_poll.io_errors++;
                    } else {
                        char old[SPDS_PATH_MAX];
                        int n=snprintf(old,sizeof old,"%s/outbox/visitor.spv",save_dir);
                        if(n>=0 && n<(int)sizeof old) remove(old);
                    }
                    mii_picker_open=0;
                } else last_poll.io_errors++;
                draw();continue;
            }
            continue;
        }
        spds_bark_tick(&bark,(keysHeld() & KEY_LID)!=0);
        if(spds_arrival_notice_tick(&recent_arrival)) draw();
        if (puzzle_open && (down & (KEY_B | KEY_X))) {
            puzzle_open=0;
            roster_open=1;
            refresh_roster();
            draw();
            continue;
        }
        if(puzzle_open && puzzle_ready) {
            int changed=0;
            if(down & KEY_LEFT) changed=spds_puzzle_move(&puzzle,-1,0) || changed;
            if(down & KEY_RIGHT) changed=spds_puzzle_move(&puzzle,1,0) || changed;
            if(down & KEY_UP) changed=spds_puzzle_move(&puzzle,0,-1) || changed;
            if(down & KEY_DOWN) changed=spds_puzzle_move(&puzzle,0,1) || changed;
            if(down & KEY_TOUCH) {
                touchPosition touch;
                touchRead(&touch);
                if(touch.px>=24 && touch.px<224 &&
                   touch.py>=32 && touch.py<104 &&
                   ((touch.py-32)%24)<16) {
                    unsigned col=(unsigned)(touch.px-24)/40u;
                    unsigned row=(unsigned)(touch.py-32)/24u;
                    changed=spds_puzzle_select(&puzzle,row*SPDS_PUZZLE_COLS+col) || changed;
                }
            }
            if(changed) puzzle_detail=0;
            if(down & KEY_A) {puzzle_detail=1;changed=1;}
            if(changed) draw();
        }
        if(room_open) {
            if(down & KEY_B) {room_open=0;roster_open=1;draw();continue;}
            if(down & (KEY_LEFT|KEY_RIGHT)) {
                spds_plaza_roster_change_page(&roster,(down & KEY_LEFT)?-1:1);
                room_selected=0;
                refresh_roster();draw();continue;
            }
            if(visitor_count && (down & (KEY_UP|KEY_DOWN))) {
                if(down & KEY_UP) room_selected=(room_selected+(unsigned)visitor_count-1u)%(unsigned)visitor_count;
                else room_selected=(room_selected+1u)%(unsigned)visitor_count;
                draw();continue;
            }
            if(visitor_count && (down & KEY_A)) {
                roster.selected=spds_plaza_roster_page(&roster)*SPDS_PLAZA_PAGE_SIZE+room_selected;
                room_open=0;roster_open=1;
                spds_plaza_roster_show_detail(&roster);draw();continue;
            }
            continue;
        }
        if (down & KEY_B) {
            if (!roster_open) break;
            if(roster.detail_open) spds_plaza_roster_hide_detail(&roster);
            else roster_open=0;
            draw();
            continue;
        }
        if(!puzzle_open && (down & KEY_X)) {
            roster_open=!roster_open;
            spds_plaza_roster_hide_detail(&roster);
            refresh_roster();
            draw();
        }
        if(roster_open && (down & KEY_START)) {
            if(spds_storage_queue_clear(save_dir)==0) waiting_visitors=0;
            room_selected=0;room_anim_frame=0;room_open=1;roster_open=0;draw();continue;
        }
        if(roster_open && (down & KEY_Y)) {
            puzzle_open=1;
            roster_open=0;
            draw();
        }
        if(roster_open) {
            int changed=0;
            if(down & KEY_LEFT) changed=spds_plaza_roster_change_page(&roster,-1);
            if(down & KEY_RIGHT) changed=spds_plaza_roster_change_page(&roster,1);
            if(down & KEY_UP) changed=spds_plaza_roster_step(&roster,-1) || changed;
            if(down & KEY_DOWN) changed=spds_plaza_roster_step(&roster,1) || changed;
            if(down & KEY_A) changed=spds_plaza_roster_show_detail(&roster) || changed;
            if(down & KEY_TOUCH) {
                touchPosition touch;
                touchRead(&touch);
                if(touch.py>=144 && touch.py<160) {
                    puzzle_open=1;
                    roster_open=0;
                    changed=1;
                } else if(touch.py>=40 && touch.py<88)
                    changed=spds_plaza_roster_select_slot(&roster,
                                      (unsigned)(touch.py-40)/16u) || changed;
                else if(touch.py>=112 && touch.py<144)
                    changed=spds_plaza_roster_change_page(&roster,
                                      touch.px<128?-1:1) || changed;
            }
            if(changed) {
                refresh_roster();
                draw();
            }
        }
        if (!puzzle_open && (down & KEY_L) && storage_ok) {
            auto_radio=0;
            spds_wifi_stop(&radio);
            spds_wifi_start(&radio, SPDS_WIFI_HOST);
            draw();
        }
        if (!puzzle_open && (down & KEY_R) && storage_ok) {
            auto_radio=0;
            spds_wifi_stop(&radio);
            spds_wifi_start(&radio, (keysHeld() & KEY_SELECT) ? SPDS_WIFI_MONITOR : SPDS_WIFI_CLIENT);
            draw();
        }
        if (!puzzle_open && (down & KEY_SELECT) && !(down & KEY_R)) {
            auto_radio=0;
            spds_wifi_stop(&radio);
            draw();
        }
        /* Only the actively running Plaza/menu can perform this polling.
         * Mode changes are intentionally slow to conserve startup overhead. */
        if(auto_radio && storage_ok && ++role_check_frames>=60) {
            role_check_frames=0;
            SpdsDiscoveryRole desired=spds_discovery_role(
                myself.sender_id,(uint32_t)time(NULL));
            enum SpdsWirelessMode target=
                desired==SPDS_DISCOVER_HOST ? SPDS_WIFI_HOST:SPDS_WIFI_CLIENT;
            if(radio.mode!=target || radio.phase==SPDS_PHASE_ERROR) {
                spds_wifi_stop(&radio);
                spds_wifi_start(&radio,target);
                draw();
            }
        }
        int radio_event=spds_wifi_tick(&radio, outgoing, incoming);
        /* Forward one cached visitor at a time; bounded hops prevent loops.
         * Do not forward our own profile or unvalidated frames. */
        if(radio.phase==SPDS_PHASE_LINKED && ++relay_send_frames>=180u) {
            uint8_t forwarded[SPDS_RELAY_SIZE];
            relay_send_frames=0;
            for(unsigned tries=0;tries<SPDS_RELAY_CACHE;tries++) {
                unsigned index=relay_send_index++%SPDS_RELAY_CACHE;
                if(spds_relay_prepare(&relay,index,forwarded) &&
                   spds_wifi_send_relay(&radio,forwarded)) {
                    spds_relay_commit_forward(&relay,index);
                    break;
                }
            }
        }
        if (radio_event==1) {
            SpdsRecord guest, accepted_guest;
            unsigned hops;
            uint32_t originated;
            if (spds_relay_decode(&guest,&hops,&originated,incoming,
                                  SPDS_RELAY_SIZE)) {
                if(guest.sender_id!=myself.sender_id) {
                    int verdict=spds_relay_accept(&relay,incoming,
                                  SPDS_RELAY_SIZE,(uint32_t)time(NULL),
                                  &accepted_guest);
                    if(verdict>=0) {
                        /* A replay still needs ACK after the first ACK is
                         * lost, but never ACK an unwritten encounter. */
                        int stored=spds_storage_receive(save_dir,&guest,
                                      myself.sender_id,(uint32_t)time(NULL));
                        if(stored==0 || stored==1) {
                            /* Only a durable encounter may be acknowledged or relayed. */
                            spds_relay_mark_durable(&relay,guest.sender_id,guest.nonce);
                            spds_wifi_queue_ack(&radio,guest.sender_id,guest.nonce);
                        }
                        if(stored==SPDS_STORAGE_QUEUE_FULL) {
                            ++last_poll.invalid; /* no ACK: peer may retry after queue clear */
                        } else if(stored==1) {
                            ++total;
                            ++last_poll.accepted;
                            save_puzzle_progress();
                            spds_arrival_notice_show(&recent_arrival,guest.name,
                                                    SPDS_ARRIVAL_DURATION_FRAMES);
                        }
                        else if(stored==0) ++last_poll.duplicates;
                        else ++last_poll.io_errors;
                    } else ++last_poll.invalid;
                }
            } else ++last_poll.invalid;
            draw();
        } else if(radio_event==2 || radio_event==3) draw();
        if (!roster_open && !puzzle_open && (down & KEY_Y)) {
            mii_picker_slot=mii_loaded && linked_mii.from_gallery?linked_mii.slot:0u;
            mii_picker_open=1;
            preview_mii_slot();draw();
        }
        if (!roster_open && !puzzle_open && (down & KEY_A)) {
            if (storage_ok) {
                myself.timestamp = (uint32_t)time(NULL);
                spds_relay_encode(outgoing,&myself,SPDS_RELAY_MAX_HOPS,
                                  (uint32_t)time(NULL));
                if (spds_storage_save_profile(save_dir, &myself) != 0)
                    last_poll.io_errors++;
                if (mii_loaded && linked_mii.features[47] &&
                    !spds_visitor_export(save_dir,&myself,&linked_mii))
                    last_poll.io_errors++;
            }
            draw();
        }
        if ((down & KEY_START) || ++frames >= 300) {
            frames = 0;
            poll_now();
            draw();
        }
    }
    spds_wifi_stop(&radio);
    if(sleep_override) enableSleep();
    return 0;
}
