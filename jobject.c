/**
 *  osd2web plugin for the Video Disk Recorder
 *
 *  jobject.c
 *
 *  (c) 2017 Jörg Wendel
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. See the file COPYING for details.
 *
 **/

//***************************************************************************
// Includes
//***************************************************************************

#include "lib/xml.h"

#include "update.h"

//***************************************************************************
// Event To Json
//***************************************************************************

int isBigField(const char* name)
{
   if (strstr(name, "description"))  return yes;
   if (strcmp(name, "actor") == 0)   return yes;
   if (strcmp(name, "other") == 0)   return yes;

   return no;
}

int event2Json(json_t* obj, const cEvent* event, const cChannel* channel,
               eTimerMatch timerMatch, int current, cOsdService::ObjectShape shape)
{
   if (channel && event)
   {
      json_t* oChannel = json_object();
      channel2Json(oChannel, channel);
      addToJson(obj, "channel", oChannel);
   }

   if (!event)
   {
      addToJson(obj, "eventid", na);
      return done;
   }

   addToJson(obj, "eventid", event->EventID());

   if (event->Schedule())
      addToJson(obj, "channelid", event->ChannelID().ToString());

   addToJson(obj, "title", event->Title());
   addToJson(obj, "shorttext", event->ShortText());
   addToJson(obj, "starttime", event->StartTime());
   addToJson(obj, "endtime", event->EndTime());
   addToJson(obj, "duration", event->Duration());
   addToJson(obj, "runningstatus", event->RunningStatus());
   addToJson(obj, "isrunning", event->IsRunning());
   addToJson(obj, "parentalrating", event->ParentalRating());
   addToJson(obj, "vps", event->Vps());
   addToJson(obj, "hastimer", event->HasTimer());
   addToJson(obj, "seen", event->Seen());

#if (defined (APIVERSNUM) && (APIVERSNUM >= 20304)) || (PATCHED)
   if (!isEmpty(event->Aux()))
   {
      cXml xml;
      int count = 0;
      json_t* oEpg2Vdr = json_object();

      if (xml.set(event->Aux()) == success)
      {
         for (XMLElement* e = xml.getFirst(); e; e = xml.getNext(e))
         {
            if (!isBigField(e->Name()) || current || shape & cOsdService::osLarge)
            {
               count++;
               addToJson(oEpg2Vdr, e->Name(), e->GetText());
            }
         }

         if (count)
            addToJson(obj, "epg2vdr", oEpg2Vdr);
      }
      else
      {
         tell(0, "Debug: Parsing of xml data in aux failed");
      }
   }
#endif

   if (current || shape & cOsdService::osLarge)
   {
      addToJson(obj, "description", event->Description());
   }

   // components

   const cComponents* components = event->Components();

   if (components && components->NumComponents())
   {
      json_t* oComponents = json_array();

      for (int i = 0; i < components->NumComponents(); i++)
      {
         json_t* oComp = json_object();

         addToJson(oComp, "stream", components->Component(i)->stream);
         addToJson(oComp, "type", components->Component(i)->type);
         addToJson(oComp, "language", components->Component(i)->language);
         addToJson(oComp, "description", components->Component(i)->description);

         json_array_append_new(oComponents, oComp);
      }

      addToJson(obj, "components", oComponents);
   }

   if (timerMatch == tmFull)
      addToJson(obj, "timermatch", "full");
   else if (timerMatch == tmPartial)
      addToJson(obj, "timermatch", "partial");
   else
      addToJson(obj, "timermatch", "none");

   return done;
}

//***************************************************************************
// Channel To Json
//***************************************************************************

int channel2Json(json_t* obj, const cChannel* channel)
{
   // { "channelid": "S19.2E-1-1019-10301", "channelname": "Das Erste HD", "channelnumber": 1 }

   addToJson(obj, "channelid", channel->GetChannelID().ToString());
   addToJson(obj, "channelname", channel->Name());
   addToJson(obj, "channelnumber", channel->Number());
   addToJson(obj, "provider", channel->Provider());

   return done;
}

//***************************************************************************
// Info of current Stream To Json
//***************************************************************************

int stream2Json(json_t* obj, const cChannel* channel)
{
   addToJson(obj, "dolbidigital", (bool)channel->Dpid(0));
   addToJson(obj, "multilang", (bool)channel->Apid(1));
   addToJson(obj, "vtx", (bool)channel->Tpid());
   addToJson(obj, "encrypted", (bool)channel->Ca());
   addToJson(obj, "radio", channel->Vpid() == 0 || channel->Vpid() == 1 || channel->Vpid() == 0x1fff);

   if (cDevice::PrimaryDevice())
   {
      int width = 0, height = 0; double aspect;

      // only set with active output device, otherwise 0

      cDevice::PrimaryDevice()->GetVideoSize(width, height, aspect);
      addToJson(obj, "videoheight", height);
      addToJson(obj, "videowidth", width);
   }

   return done;
}

//***************************************************************************
// Channels To Json
//***************************************************************************

int channels2Json(json_t* obj)
{
   GET_CHANNELS_READ(channels);

   for (const cChannel* channel = channels->First(); channel; channel = channels->Next(channel))
   {
      json_t* oChannel = json_object();
      channel2Json(oChannel, channel);
      json_array_append_new(obj, oChannel);
   }

   return success;
}

//***************************************************************************
// Recording To Json
//***************************************************************************

int recording2Json(json_t* obj, const cTimers* timers, const cRecording* recording, cOsdService::ObjectShape shape)
{
   if (!recording)
   {
      addToJson(obj, "name", "");
      return done;
   }

   addToJson(obj, "name", recording->Name());
   addToJson(obj, "folder", recording->Folder());
   addToJson(obj, "basename", recording->BaseName());
   addToJson(obj, "filename", recording->FileName());
   addToJson(obj, "title", recording->Title());
   addToJson(obj, "lengthinseconds", recording->LengthInSeconds());
   addToJson(obj, "filesizemb", recording->FileSizeMB());
   addToJson(obj, "isnew", recording->IsNew());
   addToJson(obj, "isedited", recording->IsEdited());
   addToJson(obj, "hasmarks", ((cRecording*)recording)->HasMarks()); // cast due to vdr 2.2.0 ' const' issue
   addToJson(obj, "start", recording->Start());

   if (const cRecordingInfo* info = recording->Info())
   {
      eTimerMatch timerMatch = tmNone;
      json_t* oInfo = json_object();
      json_t* oEvent = json_object();

      addToJson(oInfo, "channelid", info->ChannelID().ToString());
      addToJson(oInfo, "channelname", info->ChannelName());
      addToJson(oInfo, "framespersecond", info->FramesPerSecond());

      if (shape == cOsdService::ObjectShape::osLarge)
      {
         addToJson(oInfo, "description", info->Description());
         addToJson(oInfo, "aux", info->Aux());
      }

      if (timers)
         getTimerMatch(timers, info->GetEvent(), &timerMatch);

      event2Json(oEvent, info->GetEvent(), 0, timerMatch, no, shape);

      json_t* oImages = json_array();
      imagePaths2Json(oImages, recording->FileName());

      addToJson(obj, "images", oImages);
      addToJson(obj, "info", oInfo);
      addToJson(obj, "event", oEvent);
   }

   return success;
}

//***************************************************************************
// Timer To Json
//***************************************************************************

int timer2Json(json_t* obj, const cTimer* timer)
{
   if (!timer)
   {
      addToJson(obj, "id", na);
      return done;
   }

#if defined (APIVERSNUM) && (APIVERSNUM >= 20301)
   addToJson(obj, "id", timer->Id());
   addToJson(obj, "remote", timer->Remote());
#endif
   addToJson(obj, "recording", timer->Recording());
   addToJson(obj, "pending", timer->Pending());
   addToJson(obj, "invpsmargin", timer->InVpsMargin());
   addToJson(obj, "day", timer->Day());
   addToJson(obj, "file", timer->File());
   addToJson(obj, "aux", timer->Aux());
   addToJson(obj, "expired", timer->Expired());
   addToJson(obj, "starttime", timer->StartTime());
   addToJson(obj, "stoptime", timer->StopTime());
   addToJson(obj, "weekdays", timer->WeekDays());
   addToJson(obj, "firstday", timer->FirstDay());
   addToJson(obj, "flags", timer->Flags());

   if (timer->Channel())
   {
      json_t* oChannel = json_object();
      channel2Json(oChannel, timer->Channel());
      addToJson(obj, "channel", oChannel);
   }

   if (timer->Event())
   {
      json_t* oEvent = json_object();
      event2Json(oEvent, timer->Event(), 0, tmFull, no, cOsdService::osLarge);
      addToJson(obj, "event", oEvent);
   }

   const cEpgTimer_Interface_V1* extendetTimer = dynamic_cast<const cEpgTimer_Interface_V1*>((cTimer*)timer);

   if (extendetTimer)
   {
      char cStr[1+TB];
      json_t* oEpg2Vdr = json_object();

      addToJson(oEpg2Vdr, "id", extendetTimer->TimerId());
      addToJson(oEpg2Vdr, "vdrname", extendetTimer->VdrName());
      addToJson(oEpg2Vdr, "vdruuid", extendetTimer->VdrUuid());
      addToJson(oEpg2Vdr, "vdrrunning", extendetTimer->isVdrRunning());
      addToJson(oEpg2Vdr, "local", extendetTimer->isLocal());
      addToJson(oEpg2Vdr, "state", c2s(extendetTimer->State(), cStr));
      addToJson(oEpg2Vdr, "stateinfo", extendetTimer->StateInfo());
      addToJson(oEpg2Vdr, "action", c2s(extendetTimer->Action(), cStr));

      addToJson(obj, "epg2vdr", oEpg2Vdr);
   }
   else
   {
      tell(0, "Info: Cast to cEpgTimer_Interface_V1 failed - aussume epg2vdr not loaded");
   }

   return success;
}

//***************************************************************************
// Timer To Json
//***************************************************************************

int imagePaths2Json(json_t* obj, const char* path, const char* suffixFilter)
{
   DIR* dir;

   if (!(dir = opendir(path)))
   {
      tell(1, "Can't open directory '%s', '%s'", path, strerror(errno));
      return fail;
   }

#ifndef HAVE_READDIR_R
   dirent* pEntry;

   while ((pEntry = readdir(dir)))
#else
   dirent entry;
   dirent* pEntry = &entry;
   dirent* res;

   // deprecated but the only reentrant with old libc!

   while (readdir_r(dir, pEntry, &res) == 0 && res)
#endif
   {
      char* jpegPath = 0;
      const char* suffix = suffixOf(pEntry->d_name);

      if (dirTypeOf(path, pEntry) != DT_REG || pEntry->d_name[0] == '.')
         continue;

      if (isEmpty(suffix) || !strstr(suffixFilter, suffix))
         continue;

      asprintf(&jpegPath, "%s/%s", path, pEntry->d_name);
      json_array_append_new(obj, json_string(jpegPath));

      free(jpegPath);
   }

   closedir(dir);

   return done;
}
