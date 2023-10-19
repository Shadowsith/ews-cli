#!/bin/bash

# Creates folders hierachy test/abc in main folder inbox (if not exists)
ews-cli --url "https://owa.example.com/EWS/Exchange.asmx" \
    --domain "TEST" \
    --user "someuser" \
    --password "somepassword" \
    --action "move_folder" \
    --mailbox "support@example.com" \
    --standard-folder "inbox" \
    --folder-path "test/abc" \
    --dest-folder-path "test/xyz"