@echo off

set CFG_PROJECT=%~n0
set CODEC=1

call build.cmd

@if not defined NO_PAUSE pause
