#!/bin/bash

# get all mails in inbox
ews-cli --url "https://owa.example.com/EWS/Exchange.asmx" \
    --domain "TEST" \
    --user "someuser" \
    --password "somepassword" \
    --action "get_mails" \
    --mailbox "support@example.com" \
    --standard-folder "inbox"