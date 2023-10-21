package com.lkdw.application.applocker.Fortify;

import android.app.Service;
import android.content.Intent;
import android.os.Build;
import android.os.IBinder;
import android.util.Log;
import android.net.Uri;
import android.app.ActivityManager;
import android.app.ActivityManager.RunningTaskInfo;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.content.pm.PackageInfo;
import android.content.pm.ApplicationInfo;
import android.app.usage.UsageEvents;
import android.app.usage.UsageEvents.Event;
import android.app.usage.UsageStatsManager;
import android.app.usage.UsageStats;
import android.app.ActivityManager.RunningAppProcessInfo;
import android.content.Context;
import android.provider.Settings;
import android.app.AppOpsManager;
import android.app.ActivityManager.AppTask;
import android.app.Notification;
import android.app.PendingIntent;
import android.app.NotificationManager;
import android.app.NotificationChannel;
import androidx.core.app.NotificationCompat;

import java.util.ArrayList;
import java.util.Timer;
import java.util.TimerTask;
import java.util.Collections;
import java.util.SortedMap;
import java.util.TreeMap;
import java.util.List;

public class MyService extends Service {
    final String TAG = "MyService";
    static final String ACTION_RESTART = "com.lkdw.application.applocker.Fortify.MyService.ACTION_RESTART";
    static final String ACTION_START = "com.lkdw.application.applocker.Fortify.MyService.ACTION_START";
    static final String ACTION_STOP = "com.lkdw.application.applocker.Fortify.MyService.ACTION_STOP";
    final String NOTIFICAION_CONTENT_TITLE = "This device is safe.";
    final String NOTIFICAION_CONTENT_TEXT = "";
    final String NOTIFICAION_CHANNEL_ID = "AppLockerNotificationChannelId";
    final String NOTIFICAION_CHANNEL_NAME = "AppLockerNotificationChannel";
    final int INTERVAL = 500;

    static ArrayList<String> mLockAppsList = null;
    String mCurrentApp = "";
    int mCurrentAppState = UsageEvents.Event.ACTIVITY_PAUSED;
    Timer mTimer = null;

    public MyService() {
       Log.i(TAG, "constructor");
    }

    @Override
    public IBinder onBind(Intent intent) {
      // TO-DO: Return the communication channel to the service.
      throw new UnsupportedOperationException("Not yet implemented");
    }

    @Override
    public void onCreate() {
        super.onCreate();
        Log.i(TAG, "onCreate");

    }

    @Override
    public void onTaskRemoved(Intent rootIntent) {
        Log.i(TAG, "onTaskRemoved");
        Intent intent = new Intent(getApplicationContext(), MyService.class);
        intent.setAction(ACTION_RESTART);
        startForegroundService(intent);
        super.onTaskRemoved(rootIntent);
    }

    @Override
    public void onDestroy() {
        Log.i(TAG, "onDestroy");
        super.onDestroy();
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        String action = intent.getAction();
        switch (action) {
            case ACTION_START: {
                StartForegroundService(action);
                mLockAppsList = intent.getStringArrayListExtra("LockApps");
                Log.i(TAG, "onStartCommand ACTION_START appsList: " + mLockAppsList.size());
                //setExcludeFromRecentApps(true); //To-Do:- In samsung not working, need to investigate.
                break;
            }
            case ACTION_STOP: {
                Log.i(TAG, "onStartCommand ACTION_STOP");
                //setExcludeFromRecentApps(false);
                mLockAppsList = null;
                if (mTimer != null) {
                    mTimer.cancel();
                }
                stopForeground(true);
                break;
            }
            case ACTION_RESTART: {
                Log.i(TAG, "onStartCommand ACTION_RESTART");
                StartForegroundService(action);
                break;
            }
        }

        return START_STICKY;
    }

    private void StartForegroundService(final String start_command_action) {
        Log.d(TAG, "StartForegroundService");
        ShowNotification();
        if (start_command_action.equals(ACTION_START)) {
            if (!isUsageAccessGranted()) {
                GetusageAccessSettingsPermission();
            } else {
                launcHomeScreen();
            }
        }
        mTimer = new Timer();
        mTimer.scheduleAtFixedRate(new TimerTask() {
            @Override
            public void run() {
                Log.d(TAG, "timer triggered");
                checkForegroundTask();
            }
        }, 0, INTERVAL);
    }

    private void ShowNotification() {
        Log.d(TAG, "ShowNotification");
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            NotificationChannel serviceChannel = new NotificationChannel(
                    NOTIFICAION_CHANNEL_ID,
                    NOTIFICAION_CHANNEL_NAME,
                    NotificationManager.IMPORTANCE_LOW
            );
            serviceChannel.setDescription(NOTIFICAION_CONTENT_TEXT);
            NotificationManager manager = (NotificationManager)getSystemService(NotificationManager.class);
            manager.createNotificationChannel(serviceChannel);
        }

        Intent notificationIntent = new Intent(getApplicationContext(), MyService.class);
        PendingIntent pendingIntent = PendingIntent.getActivity(this, 0, notificationIntent, PendingIntent.FLAG_IMMUTABLE);

        Notification notification = new NotificationCompat.Builder(getApplicationContext(), NOTIFICAION_CHANNEL_ID)
                .setSmallIcon(R.drawable.ic_launcher) //Without setting icon the notification will not display any title, content text
                .setContentTitle(NOTIFICAION_CONTENT_TITLE)
                .setContentText(NOTIFICAION_CONTENT_TEXT)
                .setContentIntent(pendingIntent) //To-Do: Crash observed on click of notification.
                .setPriority(NotificationCompat.PRIORITY_MIN)
                .setCategory(NotificationCompat.CATEGORY_SERVICE)
                .setOngoing(true)
                .setAutoCancel(false)
                .build();

        startForeground(1, notification);
    }

    private void checkForegroundTask() {
        Log.d(TAG, "checkForegroundTask");
        if (mLockAppsList == null) {
            Log.e(TAG, "Failed to update check foreground task because mLockAppsList is null");
//            if (mTimer != null) {
//                mTimer.cancel();
//            }
//            stopForeground(true);
            return;
        }
        updateCurrentAppState();
        for (final String app : mLockAppsList) {
            if (!mCurrentApp.equals("com.lkdw.application.applocker.Fortify")
                    && mCurrentAppState == UsageEvents.Event.ACTIVITY_RESUMED
                    && app != null && (mCurrentApp.equals(app) || mCurrentApp.equals("com.google.android.packageinstaller"))) {
                Log.d(TAG, "Current App in foreground is locked: " + mCurrentApp + " launching home screen");
                launcHomeScreen();
                break;
            }
        }
    }

    private void updateCurrentAppState() {
        Log.d(TAG, "updateCurrentAppState");
        UsageStatsManager usm = (UsageStatsManager)getSystemService("usagestats");
        if (usm == null) {
            Log.e(TAG, "Failed to update current app status because UsageStatsManager is null");
            return;
        }
        mCurrentAppState = UsageEvents.Event.NONE;
        mCurrentApp = "";
        UsageEvents.Event currentEvent;
        UsageEvents usageEvents = usm.queryEvents(System.currentTimeMillis() - INTERVAL, System.currentTimeMillis());
        while (usageEvents != null && usageEvents.hasNextEvent()) {
            currentEvent = new UsageEvents.Event();
            usageEvents.getNextEvent(currentEvent);
            Log.d(TAG, "Current app: " + currentEvent.getPackageName() + " has an event: " + currentEvent.getEventType());
             if (currentEvent.getEventType() == UsageEvents.Event.ACTIVITY_RESUMED) {
                 Log.d(TAG, "Current running app: " + currentEvent.getPackageName() + " is in " + (currentEvent.getEventType() == UsageEvents.Event.ACTIVITY_RESUMED ? "Foreground" : "Background"));
                 mCurrentAppState = currentEvent.getEventType();
                 mCurrentApp = currentEvent.getPackageName();
                 break;
             }
         }
    }

    private void launcHomeScreen() {
        Log.d(TAG, "launcHomeScreen");
        Intent startHomescreen = new Intent();
        startHomescreen.setAction(Intent.ACTION_MAIN);
        startHomescreen.addCategory(Intent.CATEGORY_HOME);
        startHomescreen.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        startActivity(startHomescreen);
    }

    public void GetusageAccessSettingsPermission() {
        Log.d(TAG, "GetusageAccessSettingsPermission");
        Intent intent = new Intent();
        intent.setAction(Settings.ACTION_USAGE_ACCESS_SETTINGS);
        intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        Uri uri = Uri.fromParts("package", getPackageName(), null);
        intent.setData(uri);
        startActivity(intent);
    }

    private boolean isUsageAccessGranted() {
        try {
            PackageManager packageManager = getPackageManager();
            ApplicationInfo applicationInfo = packageManager.getApplicationInfo(getPackageName(), 0);
            AppOpsManager appOpsManager = (AppOpsManager) getSystemService(Context.APP_OPS_SERVICE);
            int mode = 0;
            if (android.os.Build.VERSION.SDK_INT > android.os.Build.VERSION_CODES.KITKAT) {
                mode = appOpsManager.checkOpNoThrow(AppOpsManager.OPSTR_GET_USAGE_STATS,
                        applicationInfo.uid, applicationInfo.packageName);
            }
            return (mode == AppOpsManager.MODE_ALLOWED);

        } catch (PackageManager.NameNotFoundException e) {
            return false;
        }
    }

//===========================================================================================
    //Note:- Unused method, keeping for future usage.
    private void setExcludeFromRecentApps(final boolean exclude) {
        ActivityManager am = (ActivityManager)getSystemService(Context.ACTIVITY_SERVICE);
        if (am != null) {
            List<ActivityManager.AppTask> tasks = am.getAppTasks();
            if (tasks != null && tasks.size() > 0) {
                tasks.get(0).setExcludeFromRecents(exclude);
            }
        }
    }
    private void updateForegroundTask() {
        Log.d(TAG, "updateForegroundTask");
        UsageStatsManager usm = (UsageStatsManager)getSystemService("usagestats");
        if (usm == null) {
            Log.e(TAG, "Failed to update foreground app status because UsageStatsManager is null");
            return;
        }
        long time = System.currentTimeMillis();
        List<UsageStats> appList = usm.queryUsageStats(UsageStatsManager.INTERVAL_DAILY, time - (1000*1000), time);
        if (appList != null && appList.size() > 0) {
           SortedMap<Long, UsageStats> mySortedMap = new TreeMap<Long, UsageStats>();
           for (UsageStats usageStats : appList) {
               mySortedMap.put(usageStats.getLastTimeUsed(), usageStats);
           }
           if (mySortedMap != null && !mySortedMap.isEmpty()) {
               mCurrentApp = mySortedMap.get(mySortedMap.lastKey()).getPackageName();
               Log.d(TAG, "Current App running is: " + mCurrentApp);
               //updateCurrentAppState(mySortedMap.lastKey());
            }
        }
    }
}
