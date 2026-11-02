#include "wireless.h"
#include <nds.h>
#include <dswifi9.h>
#include <string.h>

/* Independent application identifier; not a Nintendo game or CECD title ID. */
#define SPDS_WIFI_GAME_ID 0x53504453u

/*
 * IRQ callbacks must never write FAT, allocate heap, or printf.
 * One fixed pending packet is enough for the experimental two-console mode;
 * duplicate/out-of-order detection happens in the SD store.
 */
static volatile unsigned rx_ready;
static uint8_t rx_buffer[SPDS_WIRE_SIZE];

static void copy_in_irq(int base, int len) {
    if (len != (int)SPDS_WIRE_SIZE || rx_ready) return;
    Wifi_RxRawReadPacket((u32)base, SPDS_WIRE_SIZE, rx_buffer);
    rx_ready = 1;
}
static void from_host(Wifi_MPPacketType type, int base, int len) {
    if (type == WIFI_MPTYPE_DATA) copy_in_irq(base, len);
}
static void from_client(Wifi_MPPacketType type, int aid, int base, int len) {
    (void)aid;
    if (type == WIFI_MPTYPE_DATA) copy_in_irq(base, len);
}
int spds_wifi_start(SpdsWireless *r, enum SpdsWirelessMode mode) {
    if (!r || (mode != SPDS_WIFI_HOST && mode != SPDS_WIFI_CLIENT))
        return -1;
    memset(r, 0, sizeof(*r));
    rx_ready = 0;
    if (!Wifi_InitDefault(INIT_ONLY | WIFI_LOCAL_ONLY)) {
        r->phase = SPDS_PHASE_ERROR;
        r->last_error = -1;
        return -1;
    }
    r->mode = mode;
    r->phase = SPDS_PHASE_START;
    int result;
    if (mode == SPDS_WIFI_HOST) {
        Wifi_MultiplayerFromClientSetPacketHandler(from_client);
        result = Wifi_MultiplayerHostMode(1, 0, 0);
    } else {
        Wifi_MultiplayerFromHostSetPacketHandler(from_host);
        result = Wifi_MultiplayerClientMode(0);
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
        Wifi_IdleMode();
        Wifi_DisableWifi();
        Wifi_Deinit();
    }
    r->mode = SPDS_WIFI_OFF;
    r->phase = SPDS_PHASE_OFF;
    rx_ready = 0;
}
int spds_wifi_tick(SpdsWireless *r,
                   const uint8_t outgoing[SPDS_WIRE_SIZE],
                   uint8_t incoming[SPDS_WIRE_SIZE]) {
    if (!r || !outgoing || !incoming || r->mode == SPDS_WIFI_OFF) return 0;
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
    if (r->phase == SPDS_PHASE_LINKED && (++r->frames % 180u) == 0u) {
        if (r->mode == SPDS_WIFI_HOST) {
            if (Wifi_MultiplayerGetClientMask() & (1u << 1)) {
                if (Wifi_MultiplayerHostToClientDataTxFrame(
                        1, outgoing, SPDS_WIRE_SIZE) >= 0) r->sent++;
            }
        } else if (Wifi_AssocStatus() == ASSOCSTATUS_ASSOCIATED) {
            if (Wifi_MultiplayerClientToHostDataTxFrame(
                    outgoing, SPDS_WIRE_SIZE) >= 0) r->sent++;
        } else {
            Wifi_ScanMode();
            r->phase = SPDS_PHASE_SCANNING;
        }
    }
    if (rx_ready) {
        memcpy(incoming, rx_buffer, SPDS_WIRE_SIZE);
        rx_ready = 0;
        r->received++;
        return 1;
    }
    return 0;
}
