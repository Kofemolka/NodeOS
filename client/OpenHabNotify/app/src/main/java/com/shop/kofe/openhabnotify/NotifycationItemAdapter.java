package com.shop.kofe.openhabnotify;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;

/**
 * Created by Kofemolka on 2/12/2017.
 */

public class NotifycationItemAdapter extends ArrayAdapter<Notification> {
    public NotifycationItemAdapter(Context context, ArrayList<Notification> items) {
        super(context, 0, items);
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        // Get the data item for this position
        Notification item = getItem(position);
        // Check if an existing view is being reused, otherwise inflate the view
        if (convertView == null) {
            convertView = LayoutInflater.from(getContext()).inflate(R.layout.notification_item, parent, false);
        }

        // Lookup view for data population

        TextView tvTitle = (TextView) convertView.findViewById(R.id.tvTitle);
        TextView tvMessage = (TextView) convertView.findViewById(R.id.tvMsg);
        TextView tvTimeStamp = (TextView) convertView.findViewById(R.id.tvTimeStamp);

        // Populate the data into the template view using the data object
        tvTitle.setText(item.Title);
        tvMessage.setText(item.Message);

        String timestamp = new SimpleDateFormat("dd.MM.yy HH:mm:ss").format(item.Timestamp);
        tvTimeStamp.setText(timestamp);

        // Return the completed view to render on screen
        return convertView;

    }
}
