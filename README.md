# mailbang-notifier
Mailbox notifier for Openbox + isync

(add-hook 'mu4e-index-updated-hook
          '(lambda () (shell-command
                       "~/workspace/mailbang-notifier/bin/mailbang-notifier ~/.emacs.d/.mbsyncrc")))
