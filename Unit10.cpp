// ---------------------------------------------------------------------------

#include <fmx.h>
#pragma hdrstop

#include "Unit10.h"
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.fmx"
TForm10 *Form10;


// ---------------------------------------------------------------------------
__fastcall TForm10::TForm10(TComponent* Owner) : TForm(Owner)
{
	apm.async_run();
}

// ---------------------------------------------------------------------------

void __fastcall TForm10::Timer1Timer(TObject *Sender) {

   UnicodeString str;
   str.sprintf(L"%d per/sec", apm.statistics.per_second.last_press_all);
   Label3->Text = str;

   str.sprintf(L"%.02f avg/sec", apm.statistics.avg_sec.result);
   Label2->Text = str;

   str.sprintf(L"%d max/sec", apm.statistics.max_apm_sec);
   Label4->Text = str;

   str.sprintf(L"%d per/min", apm.statistics.per_min.press_all);
   Label7->Text = str;

   str.sprintf(L"%.02f avg/sec", apm.statistics.avg_minute.result);
   Label6->Text = str;

   str.sprintf(L"%d max/sec", apm.statistics.max_apm_min);
   Label5->Text = str;

   str.sprintf(L"left click %d sec", apm.statistics.per_second.left_click);
   Label8->Text = str;

   str.sprintf(L"left click %.02f avg/sec", apm.statistics.avg_sec.result_left_click);
   Label9->Text = str;

   str.sprintf(L"left click %.02f avg/min", apm.statistics.avg_minute.result_left_click);
   Label10->Text = str;

   str.sprintf(L"right click %d sec", apm.statistics.per_second.right_click);
   Label11->Text = str;

   str.sprintf(L"right click %.02f avg/sec", apm.statistics.avg_sec.result_right_click);
   Label12->Text = str;

   str.sprintf(L"right click %.02f avg/min", apm.statistics.avg_minute.result_right_click);
   Label13->Text = str;
}
// ---------------------------------------------------------------------------

void __fastcall TForm10::Button1Click(TObject *Sender)
{
	apm.statistics.reset();
}
//---------------------------------------------------------------------------

void __fastcall TForm10::FormClose(TObject *Sender, TCloseAction &Action)
{
   //  disable thread read inpute keys
	apm.close();
}
//---------------------------------------------------------------------------


