#!/bin/bash

# Stage, commit and push

git add .
printf "\nCommit message:\n> "
read message
git commit -m "$message"
git push
