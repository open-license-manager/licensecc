git filter-branch --env-filter 'if [ "$GIT_AUTHOR_EMAIL" = "contini.mailing@gmail.com" ]; then
     GIT_AUTHOR_EMAIL="1121667+gcontini@users.noreply.github.com";
     GIT_AUTHOR_NAME="gcontini";
     GIT_COMMITTER_EMAIL=$GIT_AUTHOR_EMAIL;
     GIT_COMMITTER_NAME="$GIT_AUTHOR_NAME"; fi' -- --all
