@echo off
cd /d "%~dp0"
echo ==== daily finish ritual ====
set /p MSG=Enter today's topic:
git add -A
git commit -m "%MSG%"
git push
echo ==== verify: top commit should have origin/main ====
git log --oneline -1
git status -sb
pause
