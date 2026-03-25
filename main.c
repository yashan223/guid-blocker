
#include "../pinc.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_BLOCKED_GUIDS 1024
#define GUID_LENGTH 64
#define BLOCKLIST_FILE "guidblocklist.txt"

static char blockedGuids[MAX_BLOCKED_GUIDS][GUID_LENGTH];
static int blockedCount = 0;

static int StrICmp(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        int c1 = tolower((unsigned char)*s1);
        int c2 = tolower((unsigned char)*s2);
        if (c1 != c2) return c1 - c2;
        s1++;
        s2++;
    }
    return tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
}

static void TrimString(char *str) {
    char *start = str;
    char *end;
    
    while (*start == ' ' || *start == '\t') start++;
    
    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }
    
    end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')) {
        *end = '\0';
        end--;
    }
}

static void LoadBlocklist(void) {
    fileHandle_t file;
    char buf[256];
    int read;
    
    blockedCount = 0;
    
    Plugin_FS_SV_FOpenFileRead(BLOCKLIST_FILE, &file);
    if (!file) {
        Plugin_Printf("[GuidBlocker] Blocklist file '%s' not found. Create it and add GUIDs (one per line).\n", BLOCKLIST_FILE);
        return;
    }
    
    Plugin_Printf("[GuidBlocker] Loading blocklist from '%s'...\n", BLOCKLIST_FILE);
    
    while (blockedCount < MAX_BLOCKED_GUIDS) {
        read = Plugin_FS_ReadLine(buf, sizeof(buf), file);
        
        if (read == 0) {
            break;
        }
        
        if (read == -1) {
            Plugin_PrintError("[GuidBlocker] Error reading blocklist file\n");
            break;
        }
        
        TrimString(buf);
        
        if (buf[0] == '\0' || buf[0] == '#' || buf[0] == '/') {
            continue;
        }
        
        strncpy(blockedGuids[blockedCount], buf, GUID_LENGTH - 1);
        blockedGuids[blockedCount][GUID_LENGTH - 1] = '\0';
        blockedCount++;
    }
    
    Plugin_FS_FCloseFile(file);
    Plugin_Printf("[GuidBlocker] Loaded %d blocked GUIDs\n", blockedCount);
}

static qboolean IsGuidBlocked(const char *guid) {
    int i;
    
    if (!guid || guid[0] == '\0') {
        return qfalse;
    }
    
    for (i = 0; i < blockedCount; i++) {
        if (StrICmp(blockedGuids[i], guid) == 0) {
            return qtrue;
        }
    }
    
    return qfalse;
}

// ============================================================================
// Plugin Callbacks
// ============================================================================

PCL void OnInfoRequest(pluginInfo_t *info) {
    info->handlerVersion.major = PLUGIN_HANDLER_VERSION_MAJOR;
    info->handlerVersion.minor = PLUGIN_HANDLER_VERSION_MINOR;
    
    strncpy(info->fullName, "GUID Blocker", sizeof(info->fullName) - 1);
    info->fullName[sizeof(info->fullName) - 1] = '\0';
    strncpy(info->shortDescription, "Blocks players by GUID before map load", sizeof(info->shortDescription) - 1);
    info->shortDescription[sizeof(info->shortDescription) - 1] = '\0';
    
    info->pluginVersion.major = 1;
    info->pluginVersion.minor = 0;
}

PCL int OnInit(void) {
    LoadBlocklist();
    
    Plugin_Printf("[GuidBlocker] Plugin loaded. Add GUIDs to '%s' (one per line)\n", BLOCKLIST_FILE);
    
    return 0;
}

PCL void OnPlayerGetBanStatus(baninfo_t *baninfo, char *message, int len) {
    char guidStr[128];
    
    Plugin_SteamIDToString(baninfo->playerid, guidStr, sizeof(guidStr));
    
    if (IsGuidBlocked(guidStr)) {
        strncpy(message, "You are blocked from this server.", len - 1);
        message[len - 1] = '\0';
        Plugin_Printf("[GuidBlocker] Blocked player '%s' (GUID: %s)\n", baninfo->playername, guidStr);
        return;
    }
    
    Plugin_SteamIDToString(baninfo->steamid, guidStr, sizeof(guidStr));
    
    if (IsGuidBlocked(guidStr)) {
        strncpy(message, "You are blocked from this server.", len - 1);
        message[len - 1] = '\0';
        Plugin_Printf("[GuidBlocker] Blocked player '%s' (SteamID: %s)\n", baninfo->playername, guidStr);
        return;
    }
}
