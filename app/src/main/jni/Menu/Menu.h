bool iconValid, settingsValid, initValid;

#include "Icons.hpp"

// Selected menu icon index (0 = Icon 1 default). Updated by Settings spinner feature -10
int g_menuIconIndex = 0;

//Big letter cause crash
void setText(JNIEnv *env, jobject obj, const char* text){
    //https://stackoverflow.com/a/33627640/3763113
    //A little JNI calls here. You really really need a great knowledge if you want to play with JNI stuff
    //Html.fromHtml("");
    jclass html = (*env).FindClass(OBFUSCATE("android/text/Html"));
    jmethodID fromHtml = (*env).GetStaticMethodID(html, OBFUSCATE("fromHtml"), OBFUSCATE("(Ljava/lang/String;)Landroid/text/Spanned;"));

    //setText("");
    jclass textView = (*env).FindClass(OBFUSCATE("android/widget/TextView"));
    jmethodID setText = (*env).GetMethodID(textView, OBFUSCATE("setText"), OBFUSCATE("(Ljava/lang/CharSequence;)V"));

    //Java string
    jstring jstr = (*env).NewStringUTF(text);
    (*env).CallVoidMethod(obj, setText,  (*env).CallStaticObjectMethod(html, fromHtml, jstr));
}

jstring Icon(JNIEnv *env, jobject thiz) {
    iconValid = true;
    // Selectable menu icon - default Icon 1. Index set by settings spinner (-10)
    if (g_menuIconIndex < 0 || g_menuIconIndex >= MOD_ICON_COUNT)
        g_menuIconIndex = 0;
    return env->NewStringUTF(MOD_ICONS[g_menuIconIndex]);
}

jstring IconWebViewData(JNIEnv *env, jobject thiz) {
    iconValid = true;
    //WebView support GIF animation. Upload your image or GIF on imgur.com or other sites

    // From internet (Requires android.permission.INTERNET)
    // return env->NewStringUTF(OBFUSCATE("https://i.imgur.com/SujJ85j.gif"));

    // Base64 html:
    // return env->NewStringUTF("data:image/png;base64, <encoded base64 here>");

    // To disable it, return NULL. It will use normal image above:
    // return NULL

    //return env->NewStringUTF(OBFUSCATE_KEY("https://i.imgur.com/SujJ85j.gif", 'u'));
    return NULL;
}

jobjectArray SettingsList(JNIEnv *env, jobject activityObject) {
    jobjectArray ret;

    const char *features[] = {
            OBFUSCATE("Category_Settings"),
            OBFUSCATE("-1_Toggle_Save feature preferences"),
            OBFUSCATE("-3_Toggle_Auto size vertically"),
            OBFUSCATE("Category_Menu Icon"),
            OBFUSCATE("-10_Spinner_Menu Icon_Icon 1,Icon 2,Icon 3,Icon 4,Icon 5,Icon 6,Icon 7,Icon 8,Icon 9,Icon 10,Icon 11,Icon 12,Icon 13"),
            OBFUSCATE("Category_Menu"),
            OBFUSCATE("-6_Button_<font color='red'>Close settings</font>"),
    };

    int Total_Feature = (sizeof features /
                         sizeof features[0]); //Now you dont have to manually update the number everytime;
    ret = (jobjectArray)
            env->NewObjectArray(Total_Feature, env->FindClass(OBFUSCATE("java/lang/String")),
                                env->NewStringUTF(""));
    int i;
    for (i = 0; i < Total_Feature; i++)
        env->SetObjectArrayElement(ret, i, env->NewStringUTF(features[i]));

    settingsValid = true;

    return (ret);
}
