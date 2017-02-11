package com.shop.kofe.openhabnotify;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
import android.media.RingtoneManager;
import android.net.Uri;
import android.util.Log;
import android.widget.Toast;

import com.google.firebase.messaging.FirebaseMessagingService;
import com.google.firebase.messaging.RemoteMessage;

import android.support.v4.app.NotificationCompat;

import java.util.Map;

/**
 * Created by Kofemolka on 2/11/2017.
 */

public class OpenHabFMS extends FirebaseMessagingService {
    private static final String TAG = "OpenHabFMS";
    @Override
    public void onMessageReceived(RemoteMessage remoteMessage) {
        // TODO: Handle FCM messages here.
        // If the application is in the foreground handle both data and notification messages here.
        // Also if you intend on generating your own notifications as a result of a received FCM
        // message, here is where that should be initiated.
        Map<String, String> data = remoteMessage.getData();
        String title = data.get("Title");
        String message = data.get("Message");

        Log.d(TAG, "Title: " + title);
        Log.d(TAG, "Message: " + message);

        NotificationCompat.Builder builder =
                new NotificationCompat.Builder(this)
                        .setContentTitle(title)
                        .setContentText(message)
                        .setSmallIcon(R.drawable.ic_notification)
                        .setStyle(new NotificationCompat.MessagingStyle("OpenHAB"))
                        .setDefaults(Notification.DEFAULT_SOUND | Notification.DEFAULT_VIBRATE);

        int NOTIFICATION_ID = 12345;

        Intent targetIntent = new Intent(this, MainActivity.class);
        PendingIntent contentIntent = PendingIntent.getActivity(this, 0, targetIntent, PendingIntent.FLAG_UPDATE_CURRENT);
        builder.setContentIntent(contentIntent);
        NotificationManager nManager = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
        nManager.notify(NOTIFICATION_ID, builder.build());
    }
}
