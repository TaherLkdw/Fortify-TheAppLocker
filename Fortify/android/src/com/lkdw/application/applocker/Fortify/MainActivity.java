package com.lkdw.application.applocker.Fortify;

import android.app.ActivityManager;
import android.app.ActivityManager.RunningServiceInfo;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.graphics.Bitmap;
import android.graphics.Bitmap.CompressFormat;
import android.graphics.Canvas;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.Manifest;
import android.Manifest.permission;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.provider.Settings;
import android.util.Log;
import android.util.Base64;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.lang.Integer;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Map;
import java.util.HashMap;

import com.felgo.helper.FelgoActivity;

public class MainActivity extends FelgoActivity {
    final String TAG = "MainActivity";
    private final int ACCESS_PERMISSION_MULTIPLE_REQUEST = 253;
    private final int MANAGE_OVERLAY_PERMISSION_REQUEST_CODE = 353;
    private final int OVERLAY_PERMISSION_RESULT_OK_CODE = 0;

    ArrayList<String> mLockedAppsList;
    private Intent mServiceIntent = null;
    private Map<String, String> mInstalledApps = null;
    private File mInstalledAppIconsDirectory = null;
    private String[] mMultiplePermissionRequestStrings = { Manifest.permission.READ_EXTERNAL_STORAGE,
                                                           Manifest.permission.WRITE_EXTERNAL_STORAGE,
                                                           Manifest.permission.POST_NOTIFICATIONS };
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.i(TAG, " onCreate");


        List<String> permission_type_strings_list = new ArrayList<String>();
        for ( int i = 0; i < mMultiplePermissionRequestStrings.length; ++i ) {
            if (isSelfPermissionGranted( mMultiplePermissionRequestStrings[i] ) == false) {
                permission_type_strings_list.add( mMultiplePermissionRequestStrings[i] );
            }
        }
        Log.i(TAG, " permission_type_strings_list.size(): " + permission_type_strings_list.size());
        if (permission_type_strings_list.size() > 0) {
            String[] permission_type_strings_array = (String[])permission_type_strings_list.toArray(new String[permission_type_strings_list.size()]);

            //request permission from user, which is mandatory after android 6.0
            //this is an async call which will trigger onRequestPermissionsResult after user input
            requestPermissions(permission_type_strings_array, ACCESS_PERMISSION_MULTIPLE_REQUEST);
        }
    }

    public boolean isSelfPermissionGranted(String permission) {
        // For Android < Android M, self permissions are always granted.
        boolean result = true;
        if (android.os.Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            result = checkSelfPermission(permission)
                    == PackageManager.PERMISSION_GRANTED;
        }
        return result;
    }

    @Override
    public void onResume() {
        Log.i(TAG, " onResume");
        super.onResume();
    }

    @Override
    public void onStop() {
        Log.i(TAG, " onStop");
        super.onStop();
    }
    @Override
    public void onDestroy() {
        Log.i(TAG, " onDestroy");
        super.onDestroy();
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        Log.i(TAG, " onActivityResult requestCode: " + requestCode + " resultCode: " + resultCode);
        if (requestCode == MANAGE_OVERLAY_PERMISSION_REQUEST_CODE) {
            if (resultCode == OVERLAY_PERMISSION_RESULT_OK_CODE) {
                startService(mServiceIntent);
            }
        }
    }

    public boolean IsAppLockRunning() {
        ActivityManager manager = (ActivityManager) getSystemService(Context.ACTIVITY_SERVICE);
        Class<?> serviceClass = MyService.class;
        for (RunningServiceInfo service : manager.getRunningServices(Integer.MAX_VALUE)) {
            if (serviceClass.getName().equals(service.service.getClassName())) {
                return true;
            }
        }
        return false;
    }

    void SetInstalledAppIconsDirectoryPath(final String path) {
        mInstalledAppIconsDirectory = new File(path);
    }

    public Map<String, String> GetInstalledApps() {
        if (mInstalledApps != null) {
            mInstalledApps.clear();
        }
        mInstalledApps = new HashMap<String, String>();
        PackageManager packageManager = getPackageManager();

        List<PackageInfo> packs = packageManager.getInstalledPackages(0);
        Log.i(TAG, " Total installed apps: " + packs.size());
        for (int i = 0; i < packs.size(); i++) {
            PackageInfo p = packs.get(i);
            ApplicationInfo a = p.applicationInfo;
            // skip system apps if they shall not be included
            if (((a.flags & ApplicationInfo.FLAG_SYSTEM) == 1 || (a.flags & ApplicationInfo.FLAG_UPDATED_SYSTEM_APP) == 128)
                    && packageManager.getLaunchIntentForPackage(p.packageName) == null) {
                continue;
            }
            SaveAppIcon(a);
            mInstalledApps.put(p.packageName, packageManager.getApplicationLabel(a).toString());
        }
        return mInstalledApps;
    }

    public void SaveAppIcon(ApplicationInfo appInfo) {
        PackageManager packageManager = getPackageManager();
        String fileName = packageManager.getApplicationLabel(appInfo).toString() + ".png";
        File imageFile = new File(mInstalledAppIconsDirectory, fileName);
        if (imageFile.exists()) {
            return;
        }

        Drawable drawable = packageManager.getApplicationIcon(appInfo);
        Bitmap bitmap = null;
        if (drawable != null) {
            if (drawable instanceof BitmapDrawable) {
                BitmapDrawable bitmapDrawable = (BitmapDrawable) drawable;
                bitmap = bitmapDrawable.getBitmap();
            } else {
                bitmap = Bitmap.createBitmap(drawable.getIntrinsicWidth(), drawable.getIntrinsicHeight(), Bitmap.Config.ARGB_8888);
                Canvas canvas = new Canvas(bitmap);
                drawable.setBounds(0, 0, canvas.getWidth(), canvas.getHeight());
                drawable.draw(canvas);
            }
            if (bitmap != null) {
                SaveBitmapToFile(imageFile, bitmap, Bitmap.CompressFormat.PNG, 100);
            }
        }
    }

    private void SaveBitmapToFile(File imageFile, Bitmap bm,
        Bitmap.CompressFormat format, int quality) {
        FileOutputStream fos = null;
        try {
            fos = new FileOutputStream(imageFile);
            bm.compress(format, quality, fos);
            fos.close();
        }
        catch (IOException e) {
            Log.e(TAG, e.getMessage());
            if (fos != null) {
                try {
                    fos.close();
                } catch (IOException e1) {
                    e1.printStackTrace();
                }
            }
        }
    }

    public void LockApps(final String[] appsList) {
        Log.i(TAG, "LockApps" );
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mLockedAppsList = new ArrayList<String>(Arrays.asList(appsList));
                mServiceIntent = new Intent(getApplicationContext(), MyService.class);
                mServiceIntent.setAction(MyService.ACTION_START);
                mServiceIntent.putStringArrayListExtra("LockApps", mLockedAppsList);
                if (CheckOverlayPermission()) {
                    startForegroundService(mServiceIntent);
                }
            }
        });
    }

    private boolean CheckOverlayPermission() {
        Intent intent = null;
        if (android.os.Build.VERSION.SDK_INT >= 24 && !Settings.canDrawOverlays(this)) {
            intent = new Intent(Settings.ACTION_MANAGE_OVERLAY_PERMISSION,
                    Uri.parse("package:" + getPackageName()));
            startActivityForResult(intent, MANAGE_OVERLAY_PERMISSION_REQUEST_CODE);
            return false;
        }
        return true;
    }

    public void UnlockApps() {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Log.i(TAG, "UnlockApps" );
                if (mServiceIntent == null) {
                    mServiceIntent = new Intent(getApplicationContext(), MyService.class);
                }
                mServiceIntent.setAction(MyService.ACTION_STOP);
                startForegroundService(mServiceIntent);
            }
        });
    }

    //Note:- Unused method, keeping for future usage.
    String BitmapToBase64String(Bitmap bitmap) {
        int size = bitmap.getRowBytes() * bitmap.getHeight();
        ByteBuffer byteBuffer = ByteBuffer.allocate(size);
        bitmap.copyPixelsToBuffer(byteBuffer);
        byte[] byteArray = byteBuffer.array();
        String encodedImage = Base64.encodeToString(byteArray, Base64.DEFAULT);
        return encodedImage;
    }

    private void DeleteRecursive(final File fileOrDirectory) {
        if (fileOrDirectory.isDirectory()) {
            for (File child : fileOrDirectory.listFiles()) {
                DeleteRecursive(child);
            }
        }
        fileOrDirectory.delete();
    }
}
