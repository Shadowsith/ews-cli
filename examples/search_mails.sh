#!/bin/bash

# Search all mails in folder test/abc with where subject contains "test"
ews-cli --url "https://owa.example.com/EWS/Exchange.asmx" \
    --domain "TEST" \
    --user "someuser" \
    --password "somepassword" \
    --action "search_mails" \
    --mailbox "support@example.com" \
    --standard-folder "inbox" \
    --folder-path "test/abc" \
    --item-property-path "subject" \
    --search-type "contains" \
    --search-filter "test" \
    --search-filter-type "text"