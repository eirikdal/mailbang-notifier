# mbangnotify
Mailboxscanner with notifications using isync and libnotify

Use it however you like. Personally, I use it with mu4e and emacs, like this:

```elisp
(add-hook 'mu4e-index-updated-hook
          '(lambda () (shell-command
                       "/usr/bin/mailbang-notifier ~/.mbsyncrc -i /usr/share/icons/Numix/64x64/actions/mail-message-new.svg")))
```

You could also run it with a crontab or as a timer, or trigger it using some other third party email application.
