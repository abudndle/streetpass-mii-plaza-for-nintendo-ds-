#include "wireless.h"
#include <nds.h>
#include <dswifi9.h>
#include <string.h>
#include "relay.h"
#include "streetpass_probe.h"

/* Independent application identifier; not a Nintendo game or CECD title ID. */
#define SPDS_WIFI_MAX_CLIENTS 15
#define SPDS_WIFI_GAME_ID 0x53504453u

/*
 * IRQ callbacks must never write FAT, allocate heap, or printf.
 * One fixed pending packet is enough for the experimental two-console mode;
 * duplicate/out-of-order detection happens in the SD store.
 */
static volatile unsigned rx_ready;
static volatile unsigned rx_size;
static uint8_t rx_buffer[SPDS_RELAY_SIZE];
static volatile unsigned probe_ready,probe_size;
static uint8_t probe_frame[SPDS_3DS_PROBE_MAX_FRAME];
static void probe_rx(int base,int len) {
    if(len<24 || len>(int)sizeof(probe_frame) || probe_ready) return;
    Wifi_RxRawReadPacket((u32)base,(u32)len,probe_frame);
    probe_size=(unsigned)len;
    probe_ready=1;
}

static void copy_in_irq(int base, int len) {
    if ((len != (int)SPDS_RELAY_SIZE && len != (int)SPDS_ACK_SIZE) ||
        rx_ready) return;
    Wifi_RxRawReadPacket((u32)base, len, rx_buffer);
    rx_size = (unsigned)len;
    rx_ready = 1;
}
static void from_host(Wifi_MPPacketType type, int base, int len) {
    if (type == WIFI_MPTYPE_DATA || type == WIFI_MPTYPE_CMD) copy_in_irq(base, len);
}
static void from_client(Wifi_MPPacketType type, int aid, int base, int len) {
    (void)aid;
    if (type == WIFI_MPTYPE_DATA) copy_in_irq(base, len);
}
int spds_wifi_start(SpdsWireless *r, enum SpdsWirelessMode mode) {
    if (!r || (mode != SPDS_WIFI_HOST && mode != SPDS_WIFI_CLIENT && mode != SPDS_WIFI_MONITOR))
        return -1;
    memset(r, 0, sizeof(*r));
    rx_ready = 0;
    rx_size = 0;
    probe_ready = 0;
    probe_size = 0;
    if (!Wifi_InitDefault(INIT_ONLY | WIFI_LOCAL_ONLY)) {
        r->phase = SPDS_PHASE_ERROR;
        r->last_error = -1;
        return -1;
    }
    r->mode = mode;
    r->phase = SPDS_PHASE_START;
    int result;
    if (mode == SPDS_WIFI_MONITOR) {
        Wifi_RawSetPacketHandler(probe_rx);
        Wifi_EnableWifi();
        Wifi_SetChannel(1);
        Wifi_SetPromiscuousMode(1);
        r->phase = SPDS_PHASE_MONITOR;
        return 0;
    }
    if (mode == SPDS_WIFI_HOST) {
        Wifi_MultiplayerFromClientSetPacketHandler(from_client);
        result = Wifi_MultiplayerHostMode(SPDS_WIFI_MAX_CLIENTS, SPDS_RELAY_SIZE, SPDS_RELAY_SIZE);
    } else {
        Wifi_MultiplayerFromHostSetPacketHandler(from_host);
        result = Wifi_MultiplayerClientMode(SPDS_RELAY_SIZE);
    }
    if (result < 0) {
        r->last_error = result;
        spds_wifi_stop(r);
        r->phase = SPDS_PHASE_ERROR;
        return -1;
    }
    return 0;
}
void spds_wifi_stop(SpdsWireless *r) {
    if (!r) return;
    if (r->mode != SPDS_WIFI_OFF) {
        if(r->mode == SPDS_WIFI_MONITOR) {
            Wifi_SetPromiscuousMode(0);
            Wifi_RawSetPacketHandler(NULL);
        }
        Wifi_IdleMode();
        Wifi_DisableWifi();
        Wifi_Deinit();
    }
    r->mode = SPDS_WIFI_OFF;
    r->phase = SPDS_PHASE_OFF;
    rx_ready = 0;
    rx_size = 0;
    probe_ready = 0;
    probe_size = 0;
}
int spds_wifi_queue_ack(SpdsWireless *r,uint64_t sender,uint32_t nonce) {
    if(!r || r->mode==SPDS_WIFI_OFF ||
       !spds_ack_encode(r->ack_frame,sender,nonce)) return 0;
    r->ack_pending=1;
    return 1;
}
static int tx_frame(SpdsWireless *r,const uint8_t *frame,unsigned length) {
    if(r->mode==SPDS_WIFI_HOST) {
        if(Wifi_MultiplayerGetNumClients()<=0) return 0;
        /* CMD frames are delivered to every associated client. */
        return Wifi_MultiplayerHostCmdTxFrame(frame,length)>=0;
    }
    if(r->mode==SPDS_WIFI_CLIENT &&
       Wifi_AssocStatus()==ASSOCSTATUS_ASSOCIATED)
        return Wifi_MultiplayerClientToHostDataTxFrame(frame,length)>=0;
    return 0;
}
int spds_wifi_send_relay(SpdsWireless *r,const uint8_t frame[SPDS_RELAY_SIZE]) {
    if(!r || !frame || r->phase!=SPDS_PHASE_LINKED) return 0;
    SpdsRecord record;
    unsigned hops;
    uint32_t originated;
    if(!spds_relay_decode(&record,&hops,&originated,frame,SPDS_RELAY_SIZE))
        return 0;
    if(!tx_frame(r,frame,SPDS_RELAY_SIZE)) return 0;
    ++r->sent;
    spds_ack_tracker_track(&r->ack,record.sender_id,record.nonce);
    return 1;
}
int spds_wifi_tick(SpdsWireless *r,
                   const uint8_t outgoing[SPDS_RELAY_SIZE],
                   uint8_t incoming[SPDS_RELAY_SIZE]) {
    if (!r || !outgoing || !incoming || r->mode == SPDS_WIFI_OFF) return 0;
    if(r->mode == SPDS_WIFI_MONITOR) {
        ++r->frames;
        if((r->frames % 120u)==0u) {
            static const int channels[]={1,6,11};
            Wifi_SetChannel(channels[(r->frames/120u)%3u]);
        }
        if(probe_ready) {
            unsigned n=probe_size;
            /* Raw callback only copies bytes; parse outside IRQ. */
            int recognized=spds_3ds_probe_is_streetpass(probe_frame,n);
            probe_ready=0;
            if(recognized) { ++r->probe_hits; return 3; }
        }
        return 0;
    }
    if (r->phase == SPDS_PHASE_START && Wifi_LibraryModeReady()) {
        if (r->mode == SPDS_WIFI_HOST) {
            Wifi_SetChannel(7);
            Wifi_MultiplayerAllowNewClients(true);
            if (Wifi_BeaconStart("SPDS Plaza", SPDS_WIFI_GAME_ID) < 0) {
                r->phase = SPDS_PHASE_ERROR;
                r->last_error = -2;
            } else {
                r->phase = SPDS_PHASE_LINKED;
            }
        } else {
            Wifi_ScanMode();
            r->phase = SPDS_PHASE_SCANNING;
        }
    }
    if (r->phase == SPDS_PHASE_SCANNING && (++r->frames % 30u) == 0u) {
        int num = Wifi_GetNumAP();
        for (int i = 0; i < num; ++i) {
            Wifi_AccessPoint ap;
            Wifi_GetAPData(i, &ap);
            if (ap.nintendo.game_id != SPDS_WIFI_GAME_ID ||
                !ap.nintendo.allows_connections) continue;
            if (Wifi_ConnectOpenAP(&ap) >= 0) {
                r->phase = SPDS_PHASE_CONNECTING;
                r->frames = 0;
                break;
            }
        }
    }
    if (r->phase == SPDS_PHASE_CONNECTING) {
        int status = Wifi_AssocStatus();
        if (status == ASSOCSTATUS_ASSOCIATED) {
            r->phase = SPDS_PHASE_LINKED;
            r->frames = 0;
        } else if (status == ASSOCSTATUS_CANNOTCONNECT) {
            Wifi_ScanMode();
            r->phase = SPDS_PHASE_SCANNING;
        }
    }
    if (r->phase == SPDS_PHASE_LINKED) {
        ++r->frames;
        if (r->mode==SPDS_WIFI_CLIENT &&
            Wifi_AssocStatus()!=ASSOCSTATUS_ASSOCIATED) {
            Wifi_ScanMode();
            r->phase=SPDS_PHASE_SCANNING;
        } else {
            /* One queued ACK, rate-limited to at most 5 per second.
             * IRQ callbacks never transmit or touch FAT storage. */
            if (r->ack_pending && (r->frames % 12u)==0u &&
                tx_frame(r,r->ack_frame,SPDS_ACK_SIZE)) {
                r->ack_pending=0;
                ++r->ack_sent;
            }
            if ((r->frames % 180u)==0u) {
                SpdsRecord own;
                unsigned hops;
                uint32_t originated;
                if(spds_relay_decode(&own,&hops,&originated,
                                     outgoing,SPDS_RELAY_SIZE) &&
                   tx_frame(r,outgoing,SPDS_RELAY_SIZE)) {
                    ++r->sent;
                    spds_ack_tracker_track(&r->ack,own.sender_id,own.nonce);
                }
            }
        }
    }
    if (rx_ready) {
        unsigned n=rx_size;
        memcpy(incoming,rx_buffer,n);
        rx_ready=0;
        if(n==SPDS_ACK_SIZE)
            return spds_ack_tracker_consume(&r->ack,incoming,n) ? 2 : 0;
        ++r->received;
        return 1;
    }
    return 0;
}
