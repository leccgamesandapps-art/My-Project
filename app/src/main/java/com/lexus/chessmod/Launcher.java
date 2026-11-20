package com.lexus.chessmod;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;

public class Launcher extends Service {

    private static final String TAG = "LEXUS_Launcher";
    Menu menu;

    @Override
    public void onCreate() {
        super.onCreate();
        try {
            Preferences.context = this;
            menu = new Menu(this);
            menu.SetWindowManagerWindowService();
            menu.ShowMenu();
        } catch (Throwable t) {
            Log.e(TAG, "Menu create failed", t);
            stopSelf();
        }
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        return Service.START_NOT_STICKY;
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        try {
            if (menu != null) menu.onDestroy();
        } catch (Throwable ignored) {}
    }

    @Override
    public void onTaskRemoved(Intent rootIntent) {
        super.onTaskRemoved(rootIntent);
        stopSelf();
    }
}
