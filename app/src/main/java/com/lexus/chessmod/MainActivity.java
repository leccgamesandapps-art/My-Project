package com.lexus.chessmod;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Color;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.provider.Settings;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;

/**
 * Standalone launcher.
 * Flow:
 *  - If overlay ON  -> start mod menu once (success, no reload loop)
 *  - If overlay OFF -> show error 1#doamm with [PERMISSION] [RESTART THE APP]
 */
public class MainActivity extends Activity {

    private static final int REQ_OVERLAY = 1001;
    private static boolean menuLoadedOnce = false;

    private TextView statusTv;
    private Button btnPermission;
    private Button btnRestart;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        LinearLayout root = new LinearLayout(this);
        root.setOrientation(LinearLayout.VERTICAL);
        root.setBackgroundColor(Color.parseColor("#12121a"));
        root.setPadding(48, 100, 48, 48);

        TextView title = new TextView(this);
        title.setText("LEXUS CHESS MOD");
        title.setTextColor(Color.parseColor("#FFD700"));
        title.setTextSize(26);
        root.addView(title);

        TextView ver = new TextView(this);
        ver.setText("V1.0");
        ver.setTextColor(Color.GRAY);
        ver.setTextSize(14);
        root.addView(ver);

        statusTv = new TextView(this);
        statusTv.setTextColor(Color.WHITE);
        statusTv.setTextSize(15);
        statusTv.setPadding(0, 40, 0, 30);
        root.addView(statusTv);

        btnPermission = new Button(this);
        btnPermission.setText("PERMISSION");
        btnPermission.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                openDisplayOverAppsSettings();
            }
        });
        root.addView(btnPermission);

        btnRestart = new Button(this);
        btnRestart.setText("RESTART THE APP");
        btnRestart.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                restartAppAndLoadMenu();
            }
        });
        root.addView(btnRestart);

        setContentView(root);

        // First attempt on open
        tryLoadMenuOrShowError();
    }

    @Override
    protected void onResume() {
        super.onResume();
        // Only refresh status text; do NOT auto-reload menu again
        updateStatusOnly();
    }

    private boolean hasOverlayPermission() {
        if (Build.VERSION.SDK_INT >= 23) {
            return Settings.canDrawOverlays(this);
        }
        return true;
    }

    private void updateStatusOnly() {
        if (statusTv == null) return;
        if (menuLoadedOnce) {
            statusTv.setText("MOD MENU LOADED\n\nFloating icon is on screen.\nYou can press Home.");
            btnPermission.setVisibility(View.GONE);
            btnRestart.setVisibility(View.GONE);
        } else if (hasOverlayPermission()) {
            statusTv.setText("Overlay: ON\n\nTap RESTART THE APP to load the menu.");
            btnPermission.setVisibility(View.GONE);
            btnRestart.setVisibility(View.VISIBLE);
        } else {
            statusTv.setText("Error 1#doamm\n\nMOD MENU COULDN'T LOADED\nBECAUSE DISPLAY OVER APPS DIDN'T ENABLE");
            btnPermission.setVisibility(View.VISIBLE);
            btnRestart.setVisibility(View.VISIBLE);
        }
    }

    private void tryLoadMenuOrShowError() {
        if (menuLoadedOnce) {
            updateStatusOnly();
            return;
        }

        if (!hasOverlayPermission()) {
            showError1Doamm();
            updateStatusOnly();
            return;
        }

        // Permission OK — load once
        boolean ok = startMenuOnce();
        if (!ok) {
            showError1Doamm();
        }
        updateStatusOnly();
    }

    private boolean startMenuOnce() {
        if (menuLoadedOnce) return true;
        try {
            Preferences.context = this;
            Main.Start(this);
            menuLoadedOnce = true;
            Toast.makeText(this, "LEXUS CHESS MOD loaded", Toast.LENGTH_SHORT).show();
            return true;
        } catch (Throwable t) {
            menuLoadedOnce = false;
            Toast.makeText(this, "Load failed: " + t.getClass().getSimpleName(), Toast.LENGTH_LONG).show();
            return false;
        }
    }

    private void showError1Doamm() {
        try {
            AlertDialog.Builder b = new AlertDialog.Builder(this);
            b.setTitle("Error 1#doamm");
            b.setMessage("MOD MENU COULDN'T LOADED\nBECAUSE DISPLAY OVER APPS DIDN'T ENABLE\n\n1#doamm = Display Over Apps required for Mod Menu");
            b.setCancelable(false);
            b.setPositiveButton("PERMISSION", new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    openDisplayOverAppsSettings();
                }
            });
            b.setNegativeButton("RESTART THE APP", new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    restartAppAndLoadMenu();
                }
            });
            b.show();
        } catch (Throwable ignored) {
            // Fallback: status + buttons already show the same info
        }
    }

    /**
     * Open system page for Display over other apps for this package.
     * Works across Android versions / OEMs as far as possible.
     */
    private void openDisplayOverAppsSettings() {
        try {
            // Direct overlay permission page (Android 6+)
            Intent intent = new Intent(
                    Settings.ACTION_MANAGE_OVERLAY_PERMISSION,
                    Uri.parse("package:" + getPackageName()));
            startActivityForResult(intent, REQ_OVERLAY);
            return;
        } catch (Throwable ignored) {}

        try {
            // Some devices: app details (user can open Special access from there)
            Intent intent = new Intent(Settings.ACTION_APPLICATION_DETAILS_SETTINGS);
            intent.setData(Uri.parse("package:" + getPackageName()));
            startActivity(intent);
            return;
        } catch (Throwable ignored) {}

        try {
            // Last resort: general settings
            startActivity(new Intent(Settings.ACTION_SETTINGS));
        } catch (Throwable t) {
            Toast.makeText(this, "Open Settings > Apps > LEXUS CHESS MOD > Display over other apps", Toast.LENGTH_LONG).show();
        }
    }

    /**
     * User returns after enabling permission and taps RESTART THE APP.
     * Tries load again only if not already loaded successfully.
     */
    private void restartAppAndLoadMenu() {
        menuLoadedOnce = false;
        if (!hasOverlayPermission()) {
            showError1Doamm();
            updateStatusOnly();
            Toast.makeText(this, "Still no overlay permission", Toast.LENGTH_SHORT).show();
            return;
        }
        boolean ok = startMenuOnce();
        if (!ok) {
            showError1Doamm();
        }
        updateStatusOnly();
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == REQ_OVERLAY) {
            updateStatusOnly();
            // Do not auto-start here — user must tap RESTART THE APP
            if (hasOverlayPermission() && !menuLoadedOnce) {
                Toast.makeText(this, "Permission granted. Tap RESTART THE APP", Toast.LENGTH_LONG).show();
            }
        }
    }
}
