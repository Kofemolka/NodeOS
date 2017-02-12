package com.shop.kofe.openhabnotify;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
import android.media.RingtoneManager;
import android.net.Uri;
import android.os.Handler;
import android.support.v4.app.TaskStackBuilder;
import android.util.Log;
import android.widget.Toast;

import com.google.firebase.messaging.FirebaseMessagingService;
import com.google.firebase.messaging.RemoteMessage;

import android.support.v4.app.NotificationCompat;

import java.util.Date;
import java.util.Map;

/**
 * Created by Kofemolka on 2/11/2017.
 */

public class OpenHabFMS extends FirebaseMessagingService {
    private static final String TAG = "OpenHabFMS";
    private Handler handler;

    @Override
    public void onCreate() {
        // Handler will get associated with the current thread,
        // which is the main thread.
        handler = new Handler();
        super.onCreate();
    }

    private void runOnUiThread(Runnable runnable) {
        handler.post(runnable);
    }

    @Override
    public void onMessageReceived(RemoteMessage remoteMessage) {
        // TODO: Handle FCM messages here.
        // If the application is in the foreground handle both data and notification messages here.
        // Also if you intend on generating your own notifications as a result of a received FCM
        // message, here is where that should be initiated.
        Map<String, String> data = remoteMessage.getData();

        com.shop.kofe.openhabnotify.Notification notification = new com.shop.kofe.openhabnotify.Notification();
        notification.Title = data.get("Title");
        notification.Message = data.get("Message");
        notification.Timestamp = new Date(System.currentTimeMillis());

        notify(notification);
        save(notification);
    }

    private void notify(com.shop.kofe.openhabnotify.Notification notification)
    {
        NotificationCompat.Builder builder =
                new NotificationCompat.Builder(this)
                        .setContentTitle(notification.Title)
                        .setContentText(notification.Message)
                        .setSmallIcon(R.mipmap.ic_launcher)
                        .setAutoCancel(true)
                        .setDefaults(Notification.DEFAULT_SOUND | Notification.DEFAULT_VIBRATE | Notification.DEFAULT_LIGHTS);

        int NOTIFICATION_ID = 12345;

        Intent resultIntent = new Intent(this, MainActivity.class);
        TaskStackBuilder stackBuilder = TaskStackBuilder.create(this);

        stackBuilder.addParentStack(MainActivity.class);
        stackBuilder.addNextIntent(resultIntent);
        PendingIntent resultPendingIntent =
                stackBuilder.getPendingIntent(
                        0,
                        PendingIntent.FLAG_UPDATE_CURRENT
                );

        builder.setContentIntent(resultPendingIntent);
        NotificationManager nManager = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
        nManager.notify(NOTIFICATION_ID, builder.build());
    }

    private void save(final com.shop.kofe.openhabnotify.Notification notification)
    {
        DBHandler db = new DBHandler(this);

        db.addNotification(notification);


        try {
            runOnUiThread(new Runnable() {
                final com.shop.kofe.openhabnotify.Notification not = notification;
                @Override
                public void run() {
                    MainActivity.Items.add(not);
                    MainActivity.Adapter.notifyDataSetChanged();
                }
            });
        }
        catch (Exception e)
        {
            Log.e(TAG, "save: ", e);
        }
    }
}
