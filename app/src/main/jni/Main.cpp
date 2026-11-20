#include <list>
#include <vector>
#include <string.h>
#include <pthread.h>
#include <thread>
#include <cstring>
#include <jni.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <dlfcn.h>
#include "Includes/Logger.h"
#include "Includes/obfuscate.h"
#include "Includes/Utils.h"
#include "KittyMemory/MemoryPatch.h"
#include "Menu/Setup.h"

// Change "libil2cpp.so" to the exact target library inside SFG2 4.3's lib/<abi>/ folder
#define targetLibName OBFUSCATE("libil2cpp.so")

#include "Includes/Macros.h"

enum class GameMode {
    SINGLEPLAYER,
    MULTIPLAYER
};

// Update this when entering Singleplayer or Multiplayer in SFG2 4.3
static GameMode currentMode = GameMode::SINGLEPLAYER;

// PLAYER (ME) states
static int meSpeed = 1;
static bool meRapidFire = false;
static bool meUnliBullet = false;
static bool meUnliMoney = false;
static int meJump = 1;
static bool meUnliThrowable = false;

// PLAYER (THEM) states
static int themSpeed = 1;
static bool themRapidFire = false;
static bool themUnliBullet = false;
static bool themUnliMoney = false;
static int themJump = 1;
static bool themUnliThrowable = false;

// PLAYER (BOT) states
static int botSpeed = 1;
static bool botRapidFire = false;
static bool botUnliBullet = false;
static bool botUnliMoney = false;
static int botJump = 1;
static bool botUnliThrowable = false;

// Interactive feature IDs (Category_ and RichTextView_ are not counted)
enum FeatureID {
    // PLAYER (ME) -> 0..5
    FEAT_ME_SPEED = 0,
    FEAT_ME_RAPID_FIRE = 1,
    FEAT_ME_UNLI_BULLET = 2,
    FEAT_ME_UNLI_MONEY = 3,
    FEAT_ME_JUMP = 4,
    FEAT_ME_UNLI_THROWABLE = 5,

    // PLAYER (THEM) -> 6..11
    FEAT_THEM_SPEED = 6,
    FEAT_THEM_RAPID_FIRE = 7,
    FEAT_THEM_UNLI_BULLET = 8,
    FEAT_THEM_UNLI_MONEY = 9,
    FEAT_THEM_JUMP = 10,
    FEAT_THEM_UNLI_THROWABLE = 11,

    // PLAYER (BOT) -> 12..17
    FEAT_BOT_SPEED = 12,
    FEAT_BOT_RAPID_FIRE = 13,
    FEAT_BOT_UNLI_BULLET = 14,
    FEAT_BOT_UNLI_MONEY = 15,
    FEAT_BOT_JUMP = 16,
    FEAT_BOT_UNLI_THROWABLE = 17
};

// Example hook helper: replace 0x0 with SFG2 4.3's real RVA/offset from your dump
void (*old_PlayerUpdate)(void *instance);
void PlayerUpdate(void *instance) {
    if (instance != nullptr) {
        // TODO: Detect whether `instance` is ME, THEM, or BOT in SFG2 4.3
        // Example placeholder logic:
        // bool isLocalPlayer = ...;
        // bool isBot = ...;
        // bool isOtherPlayer = !isLocalPlayer && !isBot;

        if (currentMode == GameMode::SINGLEPLAYER || currentMode == GameMode::MULTIPLAYER) {
            if (meSpeed > 1) {
                // Apply meSpeed
            }
            if (meJump > 1) {
                // Apply meJump
            }
            if (botSpeed > 1) {
                // Apply botSpeed
            }
            if (botJump > 1) {
                // Apply botJump
            }
        }

        if (currentMode == GameMode::MULTIPLAYER) {
            if (themSpeed > 1) {
                // Apply themSpeed
            }
            if (themJump > 1) {
                // Apply themJump
            }
        }
    }
    return old_PlayerUpdate(instance);
}

void *hack_thread(void *) {
    LOGI(OBFUSCATE("pthread created"));

    do {
        sleep(1);
    } while (!isLibraryLoaded(targetLibName));

    LOGI(OBFUSCATE("%s has been loaded"), (const char *) targetLibName);

#if defined(__aarch64__)
    // Replace "0x0" with your dumped SFG2 4.3 arm64 offsets
    // HOOK("0x0", PlayerUpdate, old_PlayerUpdate);
#else
    // Replace "0x0" with your dumped SFG2 4.3 armv7 offsets
    // HOOK("0x0", PlayerUpdate, old_PlayerUpdate);
#endif

    return NULL;
}

jobjectArray GetFeatureList(JNIEnv *env, jobject context) {
    jobjectArray ret;

    const char *features[] = {
            // ---------------- Category: PLAYER (ME) ----------------
            OBFUSCATE("Category_PLAYER (ME)"),
            OBFUSCATE("RichTextView_PLAYER ME - ONLY works Singleplayer and Multiplayer"),
            OBFUSCATE("SeekBar_Speed_1_10"),
            OBFUSCATE("Toggle_RapidFire"),
            OBFUSCATE("Toggle_Unli Bullet"),
            OBFUSCATE("Toggle_Unli Money"),
            OBFUSCATE("SeekBar_Jump_1_10"),
            OBFUSCATE("Toggle_Unli Throwable"),

            // ---------------- Category: PLAYER (THEM) ----------------
            OBFUSCATE("Category_PLAYER (THEM)"),
            OBFUSCATE("RichTextView_PLAYER THEM - ONLY works Multiplayer"),
            OBFUSCATE("SeekBar_Speed_1_10"),
            OBFUSCATE("Toggle_RapidFire"),
            OBFUSCATE("Toggle_Unli Bullet"),
            OBFUSCATE("Toggle_Unli Money"),
            OBFUSCATE("SeekBar_Jump_1_10"),
            OBFUSCATE("Toggle_Unli Throwable"),

            // ---------------- Category: PLAYER (BOT) ----------------
            OBFUSCATE("Category_PLAYER (BOT)"),
            OBFUSCATE("RichTextView_PLAYER BOT - ONLY works Singleplayer and Multiplayer"),
            OBFUSCATE("SeekBar_Speed_1_10"),
            OBFUSCATE("Toggle_RapidFire"),
            OBFUSCATE("Toggle_Unli Bullet"),
            OBFUSCATE("Toggle_Unli Money"),
            OBFUSCATE("SeekBar_Jump_1_10"),
            OBFUSCATE("Toggle_Unli Throwable")
    };

    int Total_Feature = (sizeof features / sizeof features[0]);
    ret = (jobjectArray)
            env->NewObjectArray(Total_Feature, env->FindClass(OBFUSCATE("java/lang/String")),
                                env->NewStringUTF(""));

    for (int i = 0; i < Total_Feature; i++)
        env->SetObjectArrayElement(ret, i, env->NewStringUTF(features[i]));

    return (ret);
}

void Changes(JNIEnv *env, jclass clazz, jobject obj,
             jint featNum, jstring featName, jint value,
             jboolean boolean, jstring str) {

    LOGD(OBFUSCATE("Feature name: %d - %s | Value: = %d | Bool: = %d | Text: = %s"), featNum,
         env->GetStringUTFChars(featName, 0), value,
         boolean, str != NULL ? env->GetStringUTFChars(str, 0) : "");

    // BE CAREFUL NOT TO ACCIDENTLY REMOVE break;

    switch (featNum) {
        // -------- PLAYER (ME) : Singleplayer & Multiplayer --------
        case FEAT_ME_SPEED:
            if (currentMode == GameMode::SINGLEPLAYER || currentMode == GameMode::MULTIPLAYER) {
                meSpeed = value;
            }
            break;
        case FEAT_ME_RAPID_FIRE:
            if (currentMode == GameMode::SINGLEPLAYER || currentMode == GameMode::MULTIPLAYER) {
                meRapidFire = boolean;
            }
            break;
        case FEAT_ME_UNLI_BULLET:
            if (currentMode == GameMode::SINGLEPLAYER || currentMode == GameMode::MULTIPLAYER) {
                meUnliBullet = boolean;
            }
            break;
        case FEAT_ME_UNLI_MONEY:
            if (currentMode == GameMode::SINGLEPLAYER || currentMode == GameMode::MULTIPLAYER) {
                meUnliMoney = boolean;
            }
            break;
        case FEAT_ME_JUMP:
            if (currentMode == GameMode::SINGLEPLAYER || currentMode == GameMode::MULTIPLAYER) {
                meJump = value;
            }
            break;
        case FEAT_ME_UNLI_THROWABLE:
            if (currentMode == GameMode::SINGLEPLAYER || currentMode == GameMode::MULTIPLAYER) {
                meUnliThrowable = boolean;
            }
            break;

        // -------- PLAYER (THEM) : Multiplayer ONLY --------
        case FEAT_THEM_SPEED:
            if (currentMode == GameMode::MULTIPLAYER) {
                themSpeed = value;
            }
            break;
        case FEAT_THEM_RAPID_FIRE:
            if (currentMode == GameMode::MULTIPLAYER) {
                themRapidFire = boolean;
            }
            break;
        case FEAT_THEM_UNLI_BULLET:
            if (currentMode == GameMode::MULTIPLAYER) {
                themUnliBullet = boolean;
            }
            break;
        case FEAT_THEM_UNLI_MONEY:
            if (currentMode == GameMode::MULTIPLAYER) {
                themUnliMoney = boolean;
            }
            break;
        case FEAT_THEM_JUMP:
            if (currentMode == GameMode::MULTIPLAYER) {
                themJump = value;
            }
            break;
        case FEAT_THEM_UNLI_THROWABLE:
            if (currentMode == GameMode::MULTIPLAYER) {
                themUnliThrowable = boolean;
            }
            break;

        // -------- PLAYER (BOT) : Singleplayer & Multiplayer --------
        case FEAT_BOT_SPEED:
            if (currentMode == GameMode::SINGLEPLAYER || currentMode == GameMode::MULTIPLAYER) {
                botSpeed = value;
            }
            break;
        case FEAT_BOT_RAPID_FIRE:
            if (currentMode == GameMode::SINGLEPLAYER || currentMode == GameMode::MULTIPLAYER) {
                botRapidFire = boolean;
            }
            break;
        case FEAT_BOT_UNLI_BULLET:
            if (currentMode == GameMode::SINGLEPLAYER || currentMode == GameMode::MULTIPLAYER) {
                botUnliBullet = boolean;
            }
            break;
        case FEAT_BOT_UNLI_MONEY:
            if (currentMode == GameMode::SINGLEPLAYER || currentMode == GameMode::MULTIPLAYER) {
                botUnliMoney = boolean;
            }
            break;
        case FEAT_BOT_JUMP:
            if (currentMode == GameMode::SINGLEPLAYER || currentMode == GameMode::MULTIPLAYER) {
                botJump = value;
            }
            break;
        case FEAT_BOT_UNLI_THROWABLE:
            if (currentMode == GameMode::SINGLEPLAYER || currentMode == GameMode::MULTIPLAYER) {
                botUnliThrowable = boolean;
            }
            break;

        default:
            break;
    }
}

__attribute__((constructor))
void lib_main() {
    pthread_t ptid;
    pthread_create(&ptid, NULL, hack_thread, NULL);
}

int RegisterMenu(JNIEnv *env) {
    JNINativeMethod methods[] = {
            {OBFUSCATE("Icon"), OBFUSCATE("()Ljava/lang/String;"), reinterpret_cast<void *>(Icon)},
            {OBFUSCATE("IconWebViewData"),  OBFUSCATE("()Ljava/lang/String;"), reinterpret_cast<void *>(IconWebViewData)},
            {OBFUSCATE("IsGameLibLoaded"),  OBFUSCATE("()Z"), reinterpret_cast<void *>(isGameLibLoaded)},
            {OBFUSCATE("Init"),  OBFUSCATE("(Landroid/content/Context;Landroid/widget/TextView;Landroid/widget/TextView;)V"), reinterpret_cast<void *>(Init)},
            {OBFUSCATE("SettingsList"),  OBFUSCATE("()[Ljava/lang/String;"), reinterpret_cast<void *>(SettingsList)},
            {OBFUSCATE("GetFeatureList"),  OBFUSCATE("()[Ljava/lang/String;"), reinterpret_cast<void *>(GetFeatureList)},
    };

    jclass clazz = env->FindClass(OBFUSCATE("com/lexus/chessmod/Menu"));
    if (!clazz)
        return JNI_ERR;
    if (env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0])) != 0)
        return JNI_ERR;
    return JNI_OK;
}

int RegisterPreferences(JNIEnv *env) {
    JNINativeMethod methods[] = {
            {OBFUSCATE("Changes"), OBFUSCATE("(Landroid/content/Context;ILjava/lang/String;IZLjava/lang/String;)V"), reinterpret_cast<void *>(Changes)},
    };
    jclass clazz = env->FindClass(OBFUSCATE("com/lexus/chessmod/Preferences"));
    if (!clazz)
        return JNI_ERR;
    if (env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0])) != 0)
        return JNI_ERR;
    return JNI_OK;
}

int RegisterMain(JNIEnv *env) {
    JNINativeMethod methods[] = {
            {OBFUSCATE("CheckOverlayPermission"), OBFUSCATE("(Landroid/content/Context;)V"), reinterpret_cast<void *>(CheckOverlayPermission)},
    };
    jclass clazz = env->FindClass(OBFUSCATE("com/lexus/chessmod/Main"));
    if (!clazz)
        return JNI_ERR;
    if (env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0])) != 0)
        return JNI_ERR;

    return JNI_OK;
}

extern "C"
JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    vm->GetEnv((void **) &env, JNI_VERSION_1_6);
    if (RegisterMenu(env) != 0)
        return JNI_ERR;
    if (RegisterPreferences(env) != 0)
        return JNI_ERR;
    if (RegisterMain(env) != 0)
        return JNI_ERR;
    return JNI_VERSION_1_6;
}
