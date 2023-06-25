#ifndef __KSIG_H__
#define __KSIG_H__

void ksig_catch();
void ksig_block();
void ksig_unblock();
void ksig_blocknone();
void ksig_pause();

void ksig_miscignore();
void ksig_bugcatch();

void ksig_pipeignore();
void ksig_pipedefault();

void ksig_termblock();
void ksig_termunblock();
void ksig_termcatch();
void ksig_termdefault();

void ksig_alarmblock();
void ksig_alarmunblock();
void ksig_alarmcatch();
void ksig_alarmdefault();

void ksig_childblock();
void ksig_childunblock();
void ksig_childcatch();
void ksig_childdefault();

void ksig_hangupblock();
void ksig_hangupunblock();
void ksig_hangupcatch();
void ksig_hangupdefault();

#endif