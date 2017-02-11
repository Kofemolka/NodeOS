#!/usr/bin/env python
# -*- coding: utf-8 -*-
from pyfcm import FCMNotification

__author__    = 'Kofemolka'
__copyright__ = 'Copyright 2017 Kofe.shop'
__license__   = """Eclipse Public License - v 1.0 (http://www.eclipse.org/legal/epl-v10.html)"""

def plugin(srv, item):
    srv.logging.debug("*** MODULE=%s: service=%s, target=%s", __file__, item.service, item.target)

    # item.config is brought in from the configuration file
    config   = item.config

    if type(config) == dict and 'apiKey' in config and config['apiKey']:
        apiKey = config['apiKey']
    if type(config) == dict and 'devID' in config and config['devID']:
        devID = config['devID']

    try:
        push_service = FCMNotification(api_key=apiKey)
        result = push_service.notify_single_device(registration_id=devID, message_title=item.title, message_body=item.message )
    except Exception, e:
        srv.logging.warning("Error sending push notification: %s" % ( str(e)))
        return False

    return True
