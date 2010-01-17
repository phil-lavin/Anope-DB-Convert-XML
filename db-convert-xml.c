/*
 *   Copyright (C) 2003-2010 Anope Team <team@anope.org>
 *   Copyright (C) 2005-2006 Florian Schulze <certus@anope.org>
 *   Copyright (C) 2008-2010 Robin Burchell <w00t@inspircd.org>
 *   Copyright (C) 2010 Phil Lavin <phil@geekshed.net>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License (see it online
 *   at http://www.gnu.org/copyleft/gpl.html) as published by the Free
 *   Software Foundation;
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 */

#include "db-convert.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

// Following function taken from http://www.lonelycactus.com/guilebook/x1355.html
// Copyright remains with the original author
// Modifications copyright Phil Lavin (2010)
/* This function converts the characters < > & and quotation marks
 * into their html entities, which are &lt; &gt; &amp; and &quot; 
 * also converts chars < ASCII 32 (space) to their &#xx; equivalent */
static char *html_entities (char const *raw_text)
{
     int raw_length;
     int out_length;
     char *out_text;
     char new_char[2];
     int i;

     assert(raw_text != (char *)NULL);

     raw_length = strlen(raw_text);

     if (raw_length == 0)
     {
	  out_text = (char *)malloc(sizeof(char));
	  out_text[0] = '\0';
     }
     else
     {
	  /* Count the number of characters needed for the output
	   * string */
	  out_length = 0;
	  for (i = 0; i < raw_length; i++)
	  {
	       if ((raw_text[i] > 0 && raw_text[i] < 32) || (raw_text[i] >= 127)) {
                    char entity[11]; // Max size 6 char + 3 digits + 1 char + \0 = 11
		    sprintf(entity, "&amp;#%d;", int(raw_text[i]));
		    out_length += strlen(entity);
	       }
	       else {
		       switch (raw_text[i]) {
			       case '<':
			       case '>':
				    out_length += 4;
				    break;
			       case '&':
				    out_length += 5;
				    break;
			       case '"':
				    out_length += 6;
				    break;
			       default:
				    out_length += 1;
		       }
	       }
	  }

	  /* Convert some tags into their HTML entities */
	  out_text = (char *)malloc((out_length + 1) * sizeof(char));
	  out_text[0] = '\0';
	  for (i = 0; i < raw_length; i++)
	  {
	       if ((raw_text[i] > 0 && raw_text[i] < 32) || (raw_text[i] >= 127)) {
		    char entity[11]; // Max size 6 char + 3 digits + 1 char + \0 = 11
		    sprintf(entity, "&amp;#%d;", int(raw_text[i]));
		    strcat(out_text, entity);
	       }
	       else {
		       switch (raw_text[i]) {
			       case '<':
				    strcat(out_text, "&lt;");
				    break;
			       case '>':
				    strcat(out_text, "&gt;");
				    break;
			       case '&':
				    strcat(out_text, "&amp;");
				    break;
			       case '"':
				    strcat (out_text, "&quot;");
				    break;
			       default:
				    new_char[0] = raw_text[i];
				    new_char[1] = '\0';
				    strcat (out_text, new_char);
				    break;
		       }
	       }
	  }
     }

     return (out_text);
}

static std::string GetLevelName(int level)
{
	switch (level)
	{
		case 0:
			return "INVITE";
		case 1:
			return "AKICK";
		case 2:
			return "SET";
		case 3:
			return "UNBAN";
		case 4:
			return "AUTOOP";
		case 5:
			return "AUTODEOP";
		case 6:
			return "AUTOVOICE";
		case 7:
			return "OPDEOP";
		case 8:
			return "LIST";
		case 9:
			return "CLEAR";
		case 10:
			return "NOJOIN";
		case 11:
			return "CHANGE";
		case 12:
			return "MEMO";
		case 13:
			return "ASSIGN";
		case 14:
			return "BADWORDS";
		case 15:
			return "NOKICK";
		case 16:
			return "FANTASIA";
		case 17:
			return "SAY";
		case 18:
			return "GREET";
		case 19:
			return "VOICEME";
		case 20:
			return "VOICE";
		case 21:
			return "GETKEY";
		case 22:
			return "AUTOHALFOP";
		case 23:
			return "AUTOPROTECT";
		case 24:
			return "OPDEOPME";
		case 25:
			return "HALFOPME";
		case 26:
			return "HALFOP";
		case 27:
			return "PROTECTME";
		case 28:
			return "PROTECT";
		case 29:
			return "KICKME";
		case 30:
			return "KICK";
		case 31:
			return "SIGNKICK";
		case 32:
			return "BANME";
		case 33:
			return "BAN";
		case 34:
			return "TOPIC";
		case 35:
			return "INFO";
		default:
			return "INVALID";
	}
}

void process_mlock_modes(std::ofstream &fs, size_t m, const std::string &ircd)
{
	/* this is the same in all protocol modules */
		if (m &        0x1) fs << "\n\t\t\t\t<mode>INVITE</mode>";        // CMODE_i
		if (m &        0x2) fs << "\n\t\t\t\t<mode>MODERATED</mode>";     // CMODE_m
		if (m &        0x4) fs << "\n\t\t\t\t<mode>NOEXTERNAL</mode>";    // CMODE_n
		if (m &        0x8) fs << "\n\t\t\t\t<mode>PRIVATE</mode>";       // CMODE_p
		if (m &       0x10) fs << "\n\t\t\t\t<mode>SECRET</mode>";        // CMODE_s
		if (m &       0x20) fs << "\n\t\t\t\t<mode>TOPIC</mode>";         // CMODE_t
		if (m &       0x40) fs << "\n\t\t\t\t<mode>KEY</mode>";           // CMODE_k
		if (m &       0x80) fs << "\n\t\t\t\t<mode>LIMIT</mode>";         // CMODE_l
		if (m &      0x200) fs << "\n\t\t\t\t<mode>REGISTERED</mode>";    // CMODE_r

	if (ircd == "unreal" || ircd == "inspircd")
	{
		if (m &      0x100) fs << "\n\t\t\t\t<mode>REGISTEREDONLY</mode>"; // CMODE_R
		if (m &      0x400) fs << "\n\t\t\t\t<mode>BLOCKCOLOR</mode>";     // CMODE_c
		if (m &     0x2000) fs << "\n\t\t\t\t<mode>NOKNOCK</mode>";        // CMODE_K
		if (m &     0x4000) fs << "\n\t\t\t\t<mode>REDIRECT</mode>";       // CMODE_L
		if (m &     0x8000) fs << "\n\t\t\t\t<mode>OPERONLY</mode>";       // CMODE_O
		if (m &    0x10000) fs << "\n\t\t\t\t<mode>NOKICK</mode>";         // CMODE_Q
		if (m &    0x20000) fs << "\n\t\t\t\t<mode>STRIPCOLOR</mode>";     // CMODE_S
		if (m &    0x80000) fs << "\n\t\t\t\t<mode>FLOOD</mode>";          // CMODE_f
		if (m &   0x100000) fs << "\n\t\t\t\t<mode>FILTER</mode>";         // CMODE_G
		if (m &   0x200000) fs << "\n\t\t\t\t<mode>NOCTCP</mode>";         // CMODE_C
		if (m &   0x400000) fs << "\n\t\t\t\t<mode>AUDITORIUM</mode>";     // CMODE_u
		if (m &   0x800000) fs << "\n\t\t\t\t<mode>SSL</mode>";            // CMODE_z
		if (m &  0x1000000) fs << "\n\t\t\t\t<mode>NONICK</mode>";         // CMODE_N
		if (m &  0x4000000) fs << "\n\t\t\t\t<mode>REGMODERATED</mode>";   // CMODE_M
	}

	if (ircd == "unreal")
	{
		if (m &      0x800) fs << "\n\t\t\t\t<mode>ADMINONLY</mode>";       // CMODE_A
		if (m &     0x1000) fs << "";                                       // old CMODE_H (removed in 3.2)
		if (m &    0x40000) fs << "\n\t\t\t\t<mode>NOINVITE</mode>";        // CMODE_f
		if (m &  0x2000000) fs << "\n\t\t\t\t<mode>NONOTICE</mode>";        // CMODE_T
		if (m &  0x8000000) fs << "\n\t\t\t\t<mode>JOINFLOOD</mode>";       // CMODE_j
	} // if (unreal)
	if (ircd == "inspircd" )
	{
		if (m &      0x800) fs << "\n\t\t\t\t<mode>ALLINVITE</mode>";        // CMODE_A
		if (m &     0x1000) fs << "\n\t\t\t\t<mode>NONOTICE</mode>";         // CMODE_T
		/* for some reason, there is no CMODE_P in 1.8.x and no CMODE_V in the 1.9.1 protocol module
		   we are ignoring this flag until we find a solution for this problem,
		   so the +V/+P mlock mode is lost on convert
		   anope 1.8: if (m &    0x40000) fs << "\n\t\t\t\t<mode>NOINVITE</mode>";         // CMODE_V
		   anope 1.9: if (m &    0x40000) fs << "\n\t\t\t\t<mode>PERM</mode>";             // CMODE_P
		*/
		if (m &  0x2000000) fs << "\n\t\t\t\t<mode>JOINFLOOD</mode>";        // CMODE_j
		if (m &  0x8000000) fs << "\n\t\t\t\t<mode>BLOCKCAPS</mode>";        // CMODE_B
		if (m & 0x10000000) fs << "\n\t\t\t\t<mode>NICKFLOOD</mode>";        // CMODE_F
		if (m & 0x20000000) fs << "";                                        // CMODE_g (mode +g <badword>) ... can't be mlocked in older version
		if (m & 0x40000000) fs << "";                                        // CMODE_J (mode +J [seconds] ... can't be mlocked in older versions
	} // if (inspircd)
}

int main(int argc, char *argv[])
{
	dbFILE *f;
	std::ofstream fs;
	std::string hashm, ircd;

	while (hashm != "md5" && hashm != "sha1" && hashm != "oldmd5" && hashm != "plain")
	{
		if (!hashm.empty())
			std::cout << "Select a valid option, thanks!" << std::endl;
		std::cout << "Which hash method did you use? (md5, sha1, oldmd5, plain)" << std::endl << "? ";
		std::cin >> hashm;
	}

	while (ircd != "bahamut" && ircd != "charybdis" && ircd != "dreamforge" && ircd != "hybrid"
				&& ircd != "inspircd" && ircd != "plexus2" && ircd != "plexus3" && ircd != "ptlink"
				&& ircd != "rageircd" && ircd != "ratbox" && ircd != "shadowircd" && ircd != "solidircd"
				&& ircd != "ultimate2" && ircd != "ultimate3" && ircd != "unreal" && ircd != "viagra")
	{
		if (!ircd.empty())
			std::cout << "Select a valid option!" << std::endl;
		std::cout << "Which IRCd did you use? (required for converting the mlock modes)" << std::endl;
		std::cout << "(bahamut, charybdis, dreamforge, hybrid, inspircd, plexus2, plexus3, ptlink," << std::endl;
		std::cout << "rageircd, ratbox, shadowircd, solidircd, ultimate2, ultimate3, unreal, viagra)" << std::endl;
		std::cout << "Your IRCd: "; std::cin >> ircd;
	}

	std::cout << "You selected " << hashm << std::endl;

	fs.open("anope.xml");
	if (!fs.is_open())
	{
		printf("\n"C_LBLUE"Could not open anope.xml for write"C_NONE"\n\n");
		exit(1);
	}

	// Start File
	fs << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>" << std::endl;
	fs << "<!DOCTYPE addressbook SYSTEM \"http://www.geekshed.net/xml/anope.dtd\">" << std::endl;
	// VERSHUN ONE
	fs << "<anope version=\"1\">" << std::endl;

	/* Section I: Nicks */
	/* Ia: First database */
	if ((f = open_db_read("NickServ", "nick.db", 14)))
	{

		NickAlias *na, **nalast, *naprev;
		NickCore *nc, **nclast, *ncprev;
		int16 tmp16;
		int32 tmp32;
		int i, j, c;

		printf("Trying to merge nicks...\n");

		/* Nick cores */
		for (i = 0; i < 1024; i++) {
			nclast = &nclists[i];
			ncprev = NULL;

			while ((c = getc_db(f)) == 1) {
				if (c != 1) {
					printf("Invalid format in nickserv db.\n");
					exit(0);
				}

				nc = (NickCore *)calloc(1, sizeof(NickCore));
				nc->aliascount = 0;
				nc->unused = 0;

				*nclast = nc;
				nclast = &nc->next;
				nc->prev = ncprev;
				ncprev = nc;

				READ(read_string(&nc->display, f));
				READ(read_buffer(nc->pass, f));
				READ(read_string(&nc->email, f));
				READ(read_string(&nc->greet, f));
				READ(read_uint32(&nc->icq, f));
				READ(read_string(&nc->url, f));
				READ(read_uint32(&nc->flags, f));
				READ(read_uint16(&nc->language, f));
				READ(read_uint16(&nc->accesscount, f));
				if (nc->accesscount) {
					char **access;
					access = (char **)calloc(sizeof(char *) * nc->accesscount, 1);
					nc->access = access;
					for (j = 0; j < nc->accesscount; j++, access++)
						READ(read_string(access, f));
				}
				READ(read_int16(&nc->memos.memocount, f));
				READ(read_int16(&nc->memos.memomax, f));
				if (nc->memos.memocount) {
					Memo *memos;
					memos = (Memo *)calloc(sizeof(Memo) * nc->memos.memocount, 1);
					nc->memos.memos = memos;
					for (j = 0; j < nc->memos.memocount; j++, memos++) {
						READ(read_uint32(&memos->number, f));
						READ(read_uint16(&memos->flags, f));
						READ(read_int32(&tmp32, f));
						memos->time = tmp32;
						READ(read_buffer(memos->sender, f));
						READ(read_string(&memos->text, f));
					}
				}
				READ(read_uint16(&nc->channelcount, f));
				READ(read_int16(&tmp16, f));
			} /* getc_db() */
			*nclast = NULL;
		} /* for() loop */

		/* Nick aliases */
		for (i = 0; i < 1024; i++) {
			char *s = NULL;

			nalast = &nalists[i];
			naprev = NULL;

			while ((c = getc_db(f)) == 1) {
				if (c != 1) {
					printf("Invalid format in nick db.\n");
					exit(0);
				}

				na = (NickAlias *)calloc(1, sizeof(NickAlias));

				READ(read_string(&na->nick, f));
				char *mask;
				char *real;
				char *quit;
				READ(read_string(&mask, f));
				READ(read_string(&real, f));
				READ(read_string(&quit, f));

				READ(read_int32(&tmp32, f));
				na->time_registered = tmp32;
				READ(read_int32(&tmp32, f));
				na->last_seen = tmp32;
				READ(read_uint16(&na->status, f));
				READ(read_string(&s, f));
				na->nc = findcore(s, 0);
				na->nc->aliascount++;
				free(s);
				free(mask);
				free(real);
				free(quit);

				*nalast = na;
				nalast = &na->next;
				na->prev = naprev;
				naprev = na;
			} /* getc_db() */
			*nalast = NULL;
		} /* for() loop */
		close_db(f); /* End of section Ia */
	}

	/* CLEAN THE CORES */
	int i;
	for (i = 0; i < 1024; i++) {
		NickCore *ncnext;
		for (NickCore *nc = nclists[i]; nc; nc = ncnext) {
			ncnext = nc->next;
			if (nc->aliascount < 1) {
				printf("Deleting core %s (%s).\n", nc->display, nc->email);
				delcore(nc);
			}
		}
	}

	head = NULL;
	if ((f = open_db_read("HostServ", "hosts.db", 3)))
	{
		int c;
		HostCore *hc;

		while ((c = getc_db(f)) == 1)
		{
			hc = (HostCore *)calloc(1, sizeof(HostCore));
			READ(read_string(&hc->nick, f));
			READ(read_string(&hc->vIdent, f));
			READ(read_string(&hc->vHost, f));
			READ(read_string(&hc->creator, f));
			READ(read_int32(&hc->time, f));

			hc->next = head;
			head = hc;
		}

		close_db(f);
	}

	fs << "\t<nickcores>" << std::endl;

	/* Nick cores */
	for (i = 0; i < 1024; i++)
	{
		NickAlias *na;
		NickCore *nc;
		char **access;
		Memo *memos;
		int j, len;
		char cpass[1000]; // Probably still far too long but hey, it's < 1KB

		for (nc = nclists[i]; nc; nc = nc->next)
		{
			if (nc->aliascount < 1)
			{
				std::cout << "Skipping core with 0 or less aliases (wtf?)" << std::endl;
				continue;
			}

			if (nc->flags & 0x80000000)
			{
				std::cout << "Skipping forbidden nick " << nc->display << std::endl;
				continue;
			}

			// Enc pass
			if (hashm == "plain")
				len = strlen(nc->pass);
			else if (hashm == "md5")
				len = 16;
			else if (hashm == "sha1")
				len = 20;
			else if (hashm == "old")
				len = 16;
			else
				len = 32;
			b64_encode(nc->pass, len, (char *)cpass, 5000);

			fs << "\t\t<nickcore>" << std::endl;
			fs << "\t\t\t<display>" << html_entities(nc->display) << "</display>" << std::endl;
			fs << "\t\t\t<password hashtype=\"" << hashm << "\">" << html_entities(cpass) << "</password>" << std::endl;

			std::cout << "Wrote account for " << nc->display << " passlen " << strlen(cpass) << std::endl;
			if (nc->email)
				fs << "\t\t\t<email>" << html_entities(nc->email) << "</email>" << std::endl;
			if (nc->greet)
				fs << "\t\t\t<greet>" << html_entities(nc->greet) << "</greet>" << std::endl;
			if (nc->icq)
				fs << "\t\t\t<icq>" << nc->icq << "</icq>" << std::endl;
			if (nc->url)
				fs << "\t\t\t<url>" << html_entities(nc->url) << "</url>" << std::endl;

			fs << "\t\t\t<accesslist>" << std::endl;

			if (nc->accesscount)
			{
				for (j = 0, access = nc->access; j < nc->accesscount; j++, access++)
					if (access)
						fs << "\t\t\t\t<access>" << html_entities(*access) << "</access>" << std::endl;
			}

			fs << "\t\t\t</accesslist>" << std::endl;

			if (nc->flags != 0) {
				fs << "\t\t\t<flags>"
					<< ((nc->flags & NI_KILLPROTECT  ) ? "\n\t\t\t\t<flag>KILLPROTECT</flag>"  : "")
					<< ((nc->flags & NI_SECURE       ) ? "\n\t\t\t\t<flag>SECURE</flag>"       : "")
					<< ((nc->flags & NI_MSG          ) ? "\n\t\t\t\t<flag>MSG</flag>"          : "")
					<< ((nc->flags & NI_MEMO_HARDMAX ) ? "\n\t\t\t\t<flag>MEMO_HARDMAX</flag>" : "")
					<< ((nc->flags & NI_MEMO_SIGNON  ) ? "\n\t\t\t\t<flag>MEMO_SIGNON</flag>"  : "")
					<< ((nc->flags & NI_MEMO_RECEIVE ) ? "\n\t\t\t\t<flag>MEMO_RECEIVE</flag>" : "")
					<< ((nc->flags & NI_PRIVATE      ) ? "\n\t\t\t\t<flag>PRIVATE</flag>"      : "")
					<< ((nc->flags & NI_HIDE_EMAIL   ) ? "\n\t\t\t\t<flag>HIDE_EMAIL</flag>"   : "")
					<< ((nc->flags & NI_HIDE_MASK    ) ? "\n\t\t\t\t<flag>HIDE_MASK</flag>"    : "")
					<< ((nc->flags & NI_HIDE_QUIT    ) ? "\n\t\t\t\t<flag>HIDE_QUIT</flag>"    : "")
					<< ((nc->flags & NI_KILL_QUICK   ) ? "\n\t\t\t\t<flag>KILL_QUICK</flag>"   : "")
					<< ((nc->flags & NI_KILL_IMMED   ) ? "\n\t\t\t\t<flag>KILL_IMMED</flag>"   : "")
					<< ((nc->flags & NI_MEMO_MAIL    ) ? "\n\t\t\t\t<flag>MEMO_MAIL</flag>"    : "")
					<< ((nc->flags & NI_HIDE_STATUS  ) ? "\n\t\t\t\t<flag>HIDE_STATUS</flag>"  : "")
					<< ((nc->flags & NI_SUSPENDED    ) ? "\n\t\t\t\t<flag>SUSPENDED</flag>"    : "")
					<< ((nc->flags & NI_AUTOOP       ) ? "\n\t\t\t\t<flag>AUTOOP</flag>"       : "")
					<< ((nc->flags & NI_FORBIDDEN    ) ? "\n\t\t\t\t<flag>FORBIDDEN</flag>"    : "") << std::endl;
				fs << "\t\t\t</flags>" << std::endl;
			}
			if (nc->memos.memocount)
			{
				fs << "\t\t\t<memos>" << std::endl;
				memos = nc->memos.memos;
				for (j = 0; j < nc->memos.memocount; j++, memos++)
				{
					fs << "\t\t\t\t<memo number=\">" << memos->number << "\">" << std::endl;
					fs << "\t\t\t\t\t<time>" << memos->time << "</time>" << std::endl;
					fs << "\t\t\t\t\t<sender>" << html_entities(memos->sender) << "</sender>" << std::endl;
					if (memos->flags != 0) {
						fs << "\t\t\t\t\t<flags>" << std::endl;
						if (memos->flags & MF_UNREAD)
							fs << "\t\t\t\t\t\t<flag>UNREAD</flag>" << std::endl;
						if (memos->flags & MF_RECEIPT)
							fs << "\t\t\t\t\t\t<flag>RECEIPT</flag>" << std::endl;
						if (memos->flags & MF_NOTIFYS)
							fs << "\t\t\t\t\t\t<flag>NOTIFYS</flag>" << std::endl;
						fs << "\t\t\t\t\t</flags>" << std::endl;
					}
					fs << "\t\t\t\t\t<body>" << html_entities(memos->text) << "</body>" << std::endl;
					fs << "\t\t\t\t</memo>" << std::endl;
				}
				fs << "\t\t\t</memos>" << std::endl;
			}

			/* we could do this in a seperate loop, I'm doing it here for tidiness. */
			fs << "\t\t\t<aliases>" << std::endl;

			for (int tmp = 0; tmp < 1024; tmp++)
			{
				for (na = nalists[tmp]; na; na = na->next)
				{
					if (!na->nc)
					{
						std::cout << "Skipping alias with no core (wtf?)" << std::endl;
						continue;
					}

					if (na->nc != nc)
						continue;

					std::cout << "Writing: " << na->nc->display << "'s nick: " << na->nick << std::endl;

					fs << "\t\t\t\t<alias>" << std::endl;
					// Removed since it doesn't change
					//fs << "\t\t\t\t\t<display>" << html_entities(na->nc->display) << "</display>" << std::endl;
					fs << "\t\t\t\t\t<nick>" << html_entities(na->nick) << "</nick>" << std::endl;
					fs << "\t\t\t\t\t<regtime>" << na->time_registered << "</regtime>" << std::endl;
					fs << "\t\t\t\t\t<seentime>" << na->last_seen << "</seentime>" << std::endl;
					if (na->last_usermask)
						fs << "\t\t\t\t\t<lastmask>" << html_entities(na->last_usermask) << "</lastmask>" << std::endl;
					if (na->last_realname)
						fs << "\t\t\t\t\t<lastrealname>" << html_entities(na->last_realname) <<  "</lastrealname>" << std::endl;
					if (na->last_quit)
						fs << "\t\t\t\t\t<lastquit>" << html_entities(na->last_quit) << "</lastquit>" << std::endl;
					if (na->status != 0)
					{
						fs << "\t\t\t\t\t<flags>" << std::endl;
						if (na->status & NS_FORBIDDEN)
							fs << "\t\t\t\t\t\t<flag>FORBIDDEN</flag>" << std::endl;
						if (na->status & NS_NO_EXPIRE)
							fs << "\t\t\t\t\t\t<flag>NOEXPIRE</flag>" << std::endl;
						fs << "\t\t\t\t\t</flags>" << std::endl;
					}

					HostCore *hc = findHostCore(na->nick);
					if (hc)
					{
						fs << "\t\t\t\t\t<vhost>" << std::endl;
						fs << "\t\t\t\t\t\t<creator>" << html_entities(hc->creator) << "</creator>" << std::endl;
						fs << "\t\t\t\t\t\t<time>" << hc->time << "</time>" << std::endl;
						fs << "\t\t\t\t\t\t<vhost>" << html_entities(hc->vHost) << "</vhost>" << std::endl;
						if (hc->vIdent)
							fs << "\t\t\t\t\t\t<vident>" << html_entities(hc->vIdent) << "</vident>" << std::endl;
						fs << "\t\t\t\t\t</vhost>" << std::endl;
					}
					fs << "\t\t\t\t</alias>" << std::endl;
				}
			}
			fs << "\t\t\t</aliases>" << std::endl;
			fs << "\t\t</nickcore>" << std::endl;
		}
	}
	fs << "\t</nickcores>" << std::endl;

	/* Section II: Bots */
	/* IIa: First database */
	if ((f = open_db_read("Botserv", "bot.db", 10))) {
		std::string input;
		int c, broken = 0;
		int32 created;
		int16 flags, chancount;
		char *nick, *user, *host, *real;

		std::cout << "Trying to convert bots..." << std::endl;

		while (input != "y" && input != "n")
		{
			std::cout << std::endl << "Are you converting a 1.9.0 database? (y/n) " << std::endl << "? ";
			std::cin >> input;
		}
		if (input == "y")
			broken = 1;

		fs << "\t<bots>" << std::endl;
		while ((c = getc_db(f)) == 1) {
			READ(read_string(&nick, f));
			READ(read_string(&user, f));
			READ(read_string(&host, f));
			READ(read_string(&real, f));
			SAFE(read_int16(&flags, f));
			READ(read_int32(&created, f));
			READ(read_int16(&chancount, f));

			if (created == 0)
				created = time(NULL); // Unfortunatley, we forgot to store the created bot time in 1.9.1+

			/* fix for the 1.9.0 broken bot.db */
			if (broken)
			{
				flags = 0;
				if (!stricmp(nick, "ChanServ"))
					flags |= BI_CHANSERV;
				if (!stricmp(nick, "BotServ"))
					flags |= BI_BOTSERV;
				if (!stricmp(nick, "HostServ"))
					flags |= BI_HOSTSERV;
				if (!stricmp(nick, "OperServ"))
					flags |= BI_OPERSERV;
				if (!stricmp(nick, "MemoServ"))
					flags |= BI_MEMOSERV;
				if (!stricmp(nick, "NickServ"))
					flags |= BI_NICKSERV;
				if (!stricmp(nick, "Global"))
					flags |= BI_GLOBAL;
			} /* end of 1.9.0 broken database fix */
			std::cout << "Writing Bot " << nick << "!" << user << "@" << host << std::endl;
			fs << "\t\t<bot>" << std::endl;
			fs << "\t\t\t<nick>" << html_entities(nick) << "</nick>" << std::endl;
			fs << "\t\t\t<user>" << html_entities(user) << "</user>" << std::endl;
			fs << "\t\t\t<host>" << html_entities(host) << "</host>" << std::endl;
			fs << "\t\t\t<created>" << created << "</created>" << std::endl;
			fs << "\t\t\t<chancount>" << chancount << "</chancount>" << std::endl;
			fs << "\t\t\t<realname>" << html_entities(real) << "</realname>" << std::endl;

			if (flags != 0) {
				fs << "\t\t\t<flags>" << std::endl;
				fs << (( flags & BI_PRIVATE  ) ? "/n\t\t\t\t<flag>PRIVATE</flag>"  : "" )
				   << (( flags & BI_CHANSERV ) ? "/n\t\t\t\t<flag>CHANSERV</flag>" : "" )
				   << (( flags & BI_BOTSERV  ) ? "/n\t\t\t\t<flag>BOTSERV</flag>"  : "" )
				   << (( flags & BI_HOSTSERV ) ? "/n\t\t\t\t<flag>HOSTSERV</flag>" : "" )
				   << (( flags & BI_OPERSERV ) ? "/n\t\t\t\t<flag>OPERSERV</flag>" : "" )
				   << (( flags & BI_MEMOSERV ) ? "/n\t\t\t\t<flag>MEMOSERV</flag>" : "" )
				   << (( flags & BI_NICKSERV ) ? "/n\t\t\t\t<flag>NICKSERV</flag>" : "" )
				   << (( flags & BI_GLOBAL   ) ? "/n\t\t\t\t<flag>GLOBAL</flag>"   : "" ) << std::endl;
				fs << "\t\t\t</flags>" << std::endl;
			}
			fs << "\t\t</bot>" << std::endl;
		}
		fs << "\t</bots>" << std::endl;
		close_db(f);
	}

	/* Section III: Chans */
	// IIIa: First database
	if ((f = open_db_read("ChanServ", "chan.db", 16)))
	{
		ChannelInfo *ci, **last, *prev;
		int c;

		printf("Trying to merge channels...\n");

		for (i = 0; i < 256; i++)
		{
			int16 tmp16;
			int32 tmp32;
			int n_levels;
			char *s;
			int n_ttb;

			last = &chanlists[i];
			prev = NULL;

			while ((c = getc_db(f)) == 1)
			{
				int j;

				if (c != 1)
				{
					printf("Invalid format in chans.db.\n");
					exit(0);
				}

				ci = (ChannelInfo *)calloc(sizeof(ChannelInfo), 1);
				*last = ci;
				last = &ci->next;
				ci->prev = prev;
				prev = ci;
				READ(read_buffer(ci->name, f));
				READ(read_string(&ci->founder, f));
				READ(read_string(&ci->successor, f));
				READ(read_buffer(ci->founderpass, f));
				READ(read_string(&ci->desc, f));
				if (!ci->desc)
					ci->desc = strdup("");
				std::cout << "Read " << ci->name << " founder " << (ci->founder ? ci->founder : "N/A") << std::endl;
				READ(read_string(&ci->url, f));
				READ(read_string(&ci->email, f));
				READ(read_int32(&tmp32, f));
				ci->time_registered = tmp32;
				READ(read_int32(&tmp32, f));
				ci->last_used = tmp32;
				READ(read_string(&ci->last_topic, f));
				READ(read_buffer(ci->last_topic_setter, f));
				READ(read_int32(&tmp32, f));
				ci->last_topic_time = tmp32;
				READ(read_uint32(&ci->flags, f));
				// Temporary flags cleanup
				ci->flags &= ~0x80000000;
				READ(read_string(&ci->forbidby, f));
				READ(read_string(&ci->forbidreason, f));
				READ(read_int16(&tmp16, f));
				ci->bantype = tmp16;
				READ(read_int16(&tmp16, f));
				n_levels = tmp16;
				ci->levels = (int16 *)calloc(36 * sizeof(*ci->levels), 1);
				for (j = 0; j < n_levels; j++)
				{
					if (j < 36)
						READ(read_int16(&ci->levels[j], f));
					else
						READ(read_int16(&tmp16, f));
				}
				READ(read_uint16(&ci->accesscount, f));
				if (ci->accesscount)
				{
					ci->access = (ChanAccess *)calloc(ci->accesscount, sizeof(ChanAccess));
					for (j = 0; j < ci->accesscount; j++)
					{
						READ(read_uint16(&ci->access[j].in_use, f));
						if (ci->access[j].in_use)
						{
							READ(read_int16(&ci->access[j].level, f));
							READ(read_string(&s, f));
							if (s)
							{
								ci->access[j].nc = findcore(s, 0);
								free(s);
							}
							if (ci->access[j].nc == NULL)
								ci->access[j].in_use = 0;
							READ(read_int32(&tmp32, f));
							ci->access[j].last_seen = tmp32;
						}
					}
				}
				else
				{
					ci->access = NULL;
				}
				READ(read_uint16(&ci->akickcount, f));
				if (ci->akickcount)
				{
					ci->akick = (AutoKick *)calloc(ci->akickcount, sizeof(AutoKick));
					for (j = 0; j < ci->akickcount; j++)
					{
						SAFE(read_uint16(&ci->akick[j].flags, f));
						if (ci->akick[j].flags & 0x0001)
						{
							SAFE(read_string(&s, f));
							if (ci->akick[j].flags & 0x0002)
							{
								ci->akick[j].u.nc = findcore(s, 0);
								if (!ci->akick[j].u.nc)
									ci->akick[j].flags &= ~0x0001;
								free(s);
							}
							else
							{
								ci->akick[j].u.mask = s;
							}
							SAFE(read_string(&s, f));
							if (ci->akick[j].flags & 0x0001)
								ci->akick[j].reason = s;
							else if (s)
								free(s);
							SAFE(read_string(&s, f));
							if (ci->akick[j].flags & 0x0001)
							{
								ci->akick[j].creator = s;
							}
							else if (s)
							{
								free(s);
							}
							SAFE(read_int32(&tmp32, f));
							if (ci->akick[j].flags & 0x0001)
								ci->akick[j].addtime = tmp32;
						}
					}
				}
				else
				{
					ci->akick = NULL;
				}
				READ(read_uint32(&ci->mlock_on, f));
				READ(read_uint32(&ci->mlock_off, f));
				READ(read_uint32(&ci->mlock_limit, f));
				READ(read_string(&ci->mlock_key, f));
				READ(read_string(&ci->mlock_flood, f));
				READ(read_string(&ci->mlock_redirect, f));
				READ(read_int16(&ci->memos.memocount, f));
				READ(read_int16(&ci->memos.memomax, f));
				if (ci->memos.memocount)
				{
					Memo *memos;
					memos = (Memo *)calloc(sizeof(Memo) * ci->memos.memocount, 1);
					ci->memos.memos = memos;
					for (j = 0; j < ci->memos.memocount; j++, memos++)
					{
						READ(read_uint32(&memos->number, f));
						READ(read_uint16(&memos->flags, f));
						READ(read_int32(&tmp32, f));
						memos->time = tmp32;
						READ(read_buffer(memos->sender, f));
						READ(read_string(&memos->text, f));
					}
				}
				READ(read_string(&ci->entry_message, f));

				// BotServ options
				READ(read_string(&ci->bi, f));
				READ(read_int32(&tmp32, f));
				ci->botflags = tmp32;
				READ(read_int16(&tmp16, f));
				n_ttb = tmp16;
				ci->ttb = (int16 *)calloc(2 * 8, 1);
				for (j = 0; j < n_ttb; j++)
				{
					if (j < 8)
						READ(read_int16(&ci->ttb[j], f));
					else
						READ(read_int16(&tmp16, f));
				}
				for (j = n_ttb; j < 8; j++)
					ci->ttb[j] = 0;
				READ(read_int16(&tmp16, f));
				ci->capsmin = tmp16;
				READ(read_int16(&tmp16, f));
				ci->capspercent = tmp16;
				READ(read_int16(&tmp16, f));
				ci->floodlines = tmp16;
				READ(read_int16(&tmp16, f));
				ci->floodsecs = tmp16;
				READ(read_int16(&tmp16, f));
				ci->repeattimes = tmp16;

				READ(read_uint16(&ci->bwcount, f));
				if (ci->bwcount)
				{
					ci->badwords = (BadWord *)calloc(ci->bwcount, sizeof(BadWord));
					for (j = 0; j < ci->bwcount; j++)
					{
						SAFE(read_uint16(&ci->badwords[j].in_use, f));
						if (ci->badwords[j].in_use)
						{
							SAFE(read_string(&ci->badwords[j].word, f));
							SAFE(read_uint16(&ci->badwords[j].type, f));
						}
					}
				}
				else
				{
					ci->badwords = NULL;
				}
			}
			*last = NULL;
		}

		close_db(f);
	}

	ChannelInfo *ci;

	fs << "\t<channels>" << std::endl;

	for (i = 0; i < 256; i++)
	{
		for (ci = chanlists[i]; ci; ci = ci->next)
		{
			int j;
			fs << "\t\t<channel>" << std::endl;
			fs << "\t\t\t<name>" << html_entities(ci->name) << "</name>" << std::endl;
			fs << "\t\t\t<timeregistered>" << ci->time_registered << "</timeregistered>" << std::endl;
			fs << "\t\t\t<lastused>" << ci->last_used << "</lastused>" << std::endl;
			fs << "\t\t\t<bantype>" << ci->bantype << "</bantype>" << std::endl;
			fs << "\t\t\t<memomax>" << ci->memos.memomax << "</memomax>" << std::endl;
			if (ci->founder)
				fs << "\t\t\t<founder>" << html_entities(ci->founder) << "</founder>" << std::endl;
			if (ci->successor)
				fs << "\t\t\t<successor>" << html_entities(ci->successor) << "</successor>" << std::endl;
			fs << "\t\t\t<levels>" << std::endl;
			for (j = 0; j < 36; j++)
			{
				fs << "\t\t\t\t<level name=\"" << GetLevelName(j) << "\">" << ci->levels[j] << "</level>" << std::endl;
			}
			fs << "\t\t\t</levels>" << std::endl;

			if (ci->flags != 0) {
				fs << "\t\t\t<flags>"
					<< ((ci->flags & CI_KEEPTOPIC     ) ? "\n\t\t\t<flag>KEEPTOPIC</flag>"     : "")
					<< ((ci->flags & CI_SECUREOPS     ) ? "\n\t\t\t<flag>SECUREOPS</flag>"     : "")
					<< ((ci->flags & CI_PRIVATE       ) ? "\n\t\t\t<flag>PRIVATE</flag>"       : "")
					<< ((ci->flags & CI_TOPICLOCK     ) ? "\n\t\t\t<flag>TOPICLOCK</flag>"     : "")
					<< ((ci->flags & CI_RESTRICTED    ) ? "\n\t\t\t<flag>RESTRICTED</flag>"    : "")
					<< ((ci->flags & CI_PEACE         ) ? "\n\t\t\t<flag>PEACE</flag>"         : "")
					<< ((ci->flags & CI_SECURE        ) ? "\n\t\t\t<flag>SECURE</flag>"        : "")
					<< ((ci->flags & CI_FORBIDDEN     ) ? "\n\t\t\t<flag>FORBIDDEN</flag>"     : "")
					<< ((ci->flags & CI_NO_EXPIRE     ) ? "\n\t\t\t<flag>NO_EXPIRE</flag>"     : "")
					<< ((ci->flags & CI_MEMO_HARDMAX  ) ? "\n\t\t\t<flag>MEMO_HARDMAX</flag>"  : "")
					<< ((ci->flags & CI_OPNOTICE      ) ? "\n\t\t\t<flag>OPNOTICE</flag>"      : "")
					<< ((ci->flags & CI_SECUREFOUNDER ) ? "\n\t\t\t<flag>SECUREFOUNDER</flag>" : "")
					<< ((ci->flags & CI_SIGNKICK      ) ? "\n\t\t\t<flag>SIGNKICK</flag>"      : "")
					<< ((ci->flags & CI_SIGNKICK_LEVEL) ? "\n\t\t\t<flag>SIGNKICKLEVEL</flag>" : "")
					<< ((ci->flags & CI_XOP           ) ? "\n\t\t\t<flag>XOP</flag>"           : "")
					<< ((ci->flags & CI_SUSPENDED     ) ? "\n\t\t\t<flag>SUSPENDED</flag>"      : "") << std::endl;
				fs << "\t\t\t</flags>" << std::endl;
			}

			if (ci->desc)
				fs << "\t\t\t<description>" << html_entities(ci->desc) << "</description>" << std::endl;
			if (ci->url)
				fs << "\t\t\t<url>" << html_entities(ci->url) << "</url>" << std::endl;
			if (ci->email)
				fs << "\t\t\t<email>" << html_entities(ci->email) << "</email>" << std::endl;
			if (ci->last_topic) { // MD CH topic <setter> <time> :topic
				fs << "\t\t\t<topic>" << std::endl;
				fs << "\t\t\t\t<setter>" << html_entities(ci->last_topic_setter) << "</setter>" << std::endl;
				fs << "\t\t\t\t<time>" << ci->last_topic_time << "</time>" << std::endl;
				fs << "\t\t\t\t<body>" << html_entities(ci->last_topic) << "</body>" << std::endl;
				fs << "\t\t\t</topic>" << std::endl;
			}
			if (ci->flags & CI_FORBIDDEN) {
				fs << "\t\t\t<forbidden>" << std::endl;
				fs << "\t\t\t\t<by>" << html_entities(ci->forbidby) << "</by>" << std::endl;
				fs << "\t\t\t\t<reason>" << html_entities(ci->forbidreason) << "</reason>" << std::endl;
				fs << "\t\t\t</forbidden>" << std::endl;
			}

			fs << "\t\t\t<accesslist>" << std::endl;
			for (j = 0; j < ci->accesscount; j++)
			{  // MD ACCESS <display> <level> <last_seen> <creator> - creator isn't in 1.9.0-1, but is in 1.9.2
				if (ci->access[j].in_use) {
					fs << "\t\t\t\t<access>" << std::endl;
					fs << "\t\t\t\t\t<display>" << html_entities(ci->access[j].nc->display) << "</display>" << std::endl;
					fs << "\t\t\t\t\t<level>" << ci->access[j].level << "</level>" << std::endl;
					fs << "\t\t\t\t\t<seen>" << ci->access[j].last_seen << "</seen>" << std::endl;
					fs << "\t\t\t\t</access>" << std::endl;
				}
			}
			fs << "\t\t\t</accesslist>" << std::endl;

			fs << "\t\t\t<akicklist>" << std::endl;
			for (j = 0; j < ci->akickcount; j++)
			{  // MD AKICK <STUCK/UNSTUCK> <NICK/MASK> <akick> <creator> <addtime> :<reason>
				if (ci->akick[j].flags & 0x0001)
				{
					fs << "\t\t\t\t<akick>" << std::endl;
					if (ci->akick[j].flags & AK_STUCK)
						fs << "\t\t\t\t\t<stuck />" << std::endl;
					if (ci->akick[j].flags & AK_ISNICK)
						fs << "\t\t\t\t\t<nick />" << std::endl;
					fs << "\t\t\t\t\t<value>" << html_entities(((ci->akick[j].flags & AK_ISNICK) ? ci->akick[j].u.nc->display : ci->akick[j].u.mask )) << "</value>" << std::endl;
					fs << "\t\t\t\t\t<creator>" << html_entities(ci->akick[j].creator) << "</creator>" << std::endl;
					fs << "\t\t\t\t\t<addtime>" << ci->akick[j].addtime << "</addtime>" << std::endl;
					if (ci->akick[j].reason)
						fs << "\t\t\t\t\t<reason>" << html_entities(ci->akick[j].reason) << "</reason>" << std::endl;
					fs << "\t\t\t\t</akick>" << std::endl;
				}
			}
			fs << "\t\t\t</akicklist>" << std::endl;

			if (ci->mlock_on)
			{
				fs << "\t\t\t<mlockon>";
				process_mlock_modes(fs, ci->mlock_on, ircd);
				fs << std::endl << "\t\t\t</mlockon>" << std::endl;
			}
			if (ci->mlock_off)
			{
				fs << "\t\t\t<mlockoff>";
				process_mlock_modes(fs, ci->mlock_off, ircd);
				fs << std::endl << "\t\t\t</mlockoff>" << std::endl;
			}
			if (ci->mlock_limit || ci->mlock_key || ci->mlock_flood || ci->mlock_redirect)
			{
				if (ci->mlock_limit)
					fs << "\t\t\t<mlocklimit>" << ci->mlock_limit << "</mlocklimit>" << std::endl;
				if (ci->mlock_key)
					fs << "\t\t\t<mlockkey>" << ci->mlock_key << "</mlockkey>" << std::endl;
				if (ci->mlock_flood)
					fs << "\t\t\t<mlockflood>" << ci->mlock_flood << "</mlockflood>" << std::endl;
				if (ci->mlock_redirect)
					fs << "\t\t\t<mlockredirect>" << ci->mlock_redirect << "</mlockredirect>" << std::endl;
			}
			if (ci->memos.memocount)
			{
				Memo *memos;
				memos = ci->memos.memos;
				fs << "\t\t\t<memos>" << std::endl;
				for (j = 0; j < ci->memos.memocount; j++, memos++)
				{
					fs << "\t\t\t\t<memo number=\">" << memos->number << "\">" << std::endl;
					fs << "\t\t\t\t\t<time>" << memos->time << "</time>" << std::endl;
					fs << "\t\t\t\t\t<sender>" << html_entities(memos->sender) << "</sender>" << std::endl;
					if (memos->flags != 0) {
						fs << "\t\t\t\t\t<flags>" << std::endl;
						if (memos->flags & MF_UNREAD)
							fs << "\t\t\t\t\t\t<flag>UNREAD</flag>" << std::endl;
						if (memos->flags & MF_RECEIPT)
							fs << "\t\t\t\t\t\t<flag>RECEIPT</flag>" << std::endl;
						if (memos->flags & MF_NOTIFYS)
							fs << "\t\t\t\t\t\t<flag>NOTIFYS</flag>" << std::endl;
						fs << "\t\t\t\t\t</flags>" << std::endl;
					}
					fs << "\t\t\t\t\t<body>" << html_entities(memos->text) << "</body>" << std::endl;
					fs << "\t\t\t\t</memo>" << std::endl;
				}
				fs << "\t\t\t</memos>" << std::endl;
			}

			if (ci->entry_message)
				fs << "\t\t\t<entrymsg>" << html_entities(ci->entry_message) << "</entrymsg>" << std::endl;
			if (ci->bi)  // here is "bi" a *Char, not a pointer to BotInfo !
				fs << "\t\t\t<botnick>" << html_entities(ci->bi) << "</botnick>" << std::endl;

			if (ci->botflags) {
				fs << "\t\t\t<botflags>"
					<< (( ci->botflags & BS_DONTKICKOPS     ) ? "\n\t\t\t\t<flag>DONTKICKOPS</flag>"    : "" )
					<< (( ci->botflags & BS_DONTKICKVOICES  ) ? "\n\t\t\t\t<flag>DONTKICKVOICES</flag>"  : "")
					<< (( ci->botflags & BS_FANTASY         ) ? "\n\t\t\t\t<flag>FANTASY</flag>"         : "")
					<< (( ci->botflags & BS_SYMBIOSIS       ) ? "\n\t\t\t\t<flag>SYMBIOSIS</flag>"       : "")
					<< (( ci->botflags & BS_GREET           ) ? "\n\t\t\t\t<flag>GREET</flag>"           : "")
					<< (( ci->botflags & BS_NOBOT           ) ? "\n\t\t\t\t<flag>NOBOT</flag>"           : "")
					<< (( ci->botflags & BS_KICK_BOLDS      ) ? "\n\t\t\t\t<flag>KICK_BOLDS</flag>"      : "")
					<< (( ci->botflags & BS_KICK_COLORS     ) ? "\n\t\t\t\t<flag>KICK_COLORS</flag>"     : "")
					<< (( ci->botflags & BS_KICK_REVERSES   ) ? "\n\t\t\t\t<flag>KICK_REVERSES</flag>"   : "")
					<< (( ci->botflags & BS_KICK_UNDERLINES ) ? "\n\t\t\t\t<flag>KICK_UNDERLINES</flag>" : "")
					<< (( ci->botflags & BS_KICK_BADWORDS   ) ? "\n\t\t\t\t<flag>KICK_BADWORDS</flag>"   : "")
					<< (( ci->botflags & BS_KICK_CAPS       ) ? "\n\t\t\t\t<flag>KICK_CAPS</flag>"       : "")
					<< (( ci->botflags & BS_KICK_FLOOD      ) ? "\n\t\t\t\t<flag>KICK_FLOOD</flag>"      : "")
					<< (( ci->botflags & BS_KICK_REPEAT     ) ? "\n\t\t\t\t<flag>KICK_REPEAT</flag>"     : "") << std::endl;
				fs << "\t\t\t</botflags>" << std::endl;
			}

			fs << "\t\t\t<ttbs>" << std::endl;
			fs << "\t\t\t\t<ttb type=\"bolds\">" << ci->ttb[0] << "</ttb>" << std::endl;
			fs << "\t\t\t\t<ttb type=\"colors\">" << ci->ttb[1] << "</ttb>" << std::endl;
			fs << "\t\t\t\t<ttb type=\"reverses\">" << ci->ttb[2] << "</ttb>" << std::endl;
			fs << "\t\t\t\t<ttb type=\"underlines\">" << ci->ttb[3] << "</ttb>" << std::endl;
			fs << "\t\t\t\t<ttb type=\"badwords\">" << ci->ttb[4] << "</ttb>" << std::endl;
			fs << "\t\t\t\t<ttb type=\"caps\">" << ci->ttb[5] << "</ttb>" << std::endl;
			fs << "\t\t\t\t<ttb type=\"flood\">" << ci->ttb[6] << "</ttb>" << std::endl;
			fs << "\t\t\t\t<ttb type=\"repeat\">" << ci->ttb[7] << "</ttb>" << std::endl;
			fs << "\t\t\t</ttbs>" << std::endl;
			if (ci->capsmin)
				fs << "\t\t\t<capsmin>" << ci->capsmin << "</capsmin>" << std::endl;
			if (ci->capspercent)
				fs << "\t\t\t<capspercent>" << ci->capspercent << "</capspercent>" << std::endl;
			if (ci->floodlines)
				fs << "\t\t\t<floodlines>" << ci->floodlines << "</floodlines>" << std::endl;
			if (ci->floodsecs)
				fs << "\t\t\t<floodsecs>" << ci->floodsecs << "</floodsecs>" << std::endl;
			if (ci->repeattimes)
				fs << "\t\t\t<repeattimes>" << ci->repeattimes << "</repeattimes>" << std::endl;

			fs << "\t\t\t<badwords>" << std::endl;
			for (j = 0; j < ci->bwcount; j++)
			{
				if (ci->badwords[j].in_use)
				{
					fs << "\t\t\t\t<badword type=\""
						<< (( ci->badwords[j].type == 0 ) ? "any"    : "" )
						<< (( ci->badwords[j].type == 1 ) ? "single" : "" )
						<< (( ci->badwords[j].type == 3 ) ? "start"  : "" )
						<< (( ci->badwords[j].type == 4 ) ? "end"    : "" )
						<< "\">" << html_entities(ci->badwords[j].word) << "</badword>" << std::endl;
				}
			}
			fs << "\t\t\t</badwords>" << std::endl;

			fs << "\t\t</channel>" << std::endl;
		} /* for (chanlists[i]) */
	} /* for (i) */
	fs << "\t</channels>" << std::endl;

	/*********************************/
	/*    OPERSERV Section           */
	/*********************************/

	if ((f = open_db_read("OperServ", "oper.db", 13)))
	{
		int32 maxusercnt = 0, maxusertime = 0, seton = 0, expires = 0;
		int16 capacity = 0;
		char *user, *host, *by, *reason, *mask;

		std::cout << "Writing operserv data (stats, akills, sglines, szlines)" << std::endl;

		SAFE(read_int32(&maxusercnt, f));
		SAFE(read_int32(&maxusertime, f));
		fs << "\t<stats>" << std::endl;
		fs << "\t\t<maxusercount>" << maxusercnt << "</maxusercount>" << std::endl;
		fs << "\t\t<maxusertime>" << maxusertime << "</maxusertime>" << std::endl;
		fs << "\t</stats>" << std::endl;

		/* AKILLS */
		read_int16(&capacity, f);
		fs << "\t<akills>" << std::endl;
		for (i = 0; i < capacity; i++)
		{
			SAFE(read_string(&user, f));
			SAFE(read_string(&host, f));
			SAFE(read_string(&by, f));
			SAFE(read_string(&reason, f));
			SAFE(read_int32(&seton, f));
			SAFE(read_int32(&expires, f));
			fs << "\t\t<akill>" << std::endl;
			fs << "\t\t\t<user>" << html_entities(user) << "</user>" << std::endl;
			fs << "\t\t\t<host>" << html_entities(host) << "</host>" << std::endl;
			fs << "\t\t\t<by>" << html_entities(by) << "</by>" << std::endl;
			fs << "\t\t\t<seton>" << seton << "</seton>" << std::endl;
			fs << "\t\t\t<expires>" << expires << "</expires>" << std::endl;
			fs << "\t\t\t<reason>" << html_entities(reason) << "</reason>" << std::endl;
			fs << "\t\t</akill>" << std::endl;
			free(user); free(host); free(by); free(reason);
		}
		fs << "\t</akills>" << std::endl;
		/* SGLINES */
		read_int16(&capacity, f);
		fs << "\t<sglines>" << std::endl;
		for (i = 0; i < capacity; i++)
		{
			SAFE(read_string(&mask, f));
			SAFE(read_string(&by, f));
			SAFE(read_string(&reason, f));
			SAFE(read_int32(&seton, f));
			SAFE(read_int32(&expires, f));
			fs << "\t\t<sgline>" << std::endl;
			fs << "\t\t\t<mask>" << html_entities(mask) << "</mask>" << std::endl;
			fs << "\t\t\t<by>" << html_entities(by) << "</by>" << std::endl;
			fs << "\t\t\t<seton>" << seton << "</seton>" << std::endl;
			fs << "\t\t\t<expires>" << expires << "</expires>" << std::endl;
			fs << "\t\t\t<reason>" << html_entities(reason) << "</reason>" << std::endl;
			fs << "\t\t</sgline>" << std::endl;
			free(mask); free(by); free(reason);
		}
		fs << "\t</sglines>" << std::endl;
		/* SQLINES */
		read_int16(&capacity, f);
		fs << "\t<sqlines>" << std::endl;
		for (i = 0; i < capacity; i++)
		{
			SAFE(read_string(&mask, f));
			SAFE(read_string(&by, f));
			SAFE(read_string(&reason, f));
			SAFE(read_int32(&seton, f));
			SAFE(read_int32(&expires, f));
			fs << "\t\t<sqline>" << std::endl;
			fs << "\t\t\t<mask>" << html_entities(mask) << "</mask>" << std::endl;
			fs << "\t\t\t<by>" << html_entities(by) << "</by>" << std::endl;
			fs << "\t\t\t<seton>" << seton << "</seton>" << std::endl;
			fs << "\t\t\t<expires>" << expires << "</expires>" << std::endl;
			fs << "\t\t\t<reason>" << html_entities(reason) << "</reason>" << std::endl;
			fs << "\t\t</sqline>" << std::endl;
			free(mask); free(by); free(reason);
		}
		fs << "\t</sqlines>" << std::endl;
		/* SZLINES */
		read_int16(&capacity, f);
		fs << "\t<szlines>" << std::endl;
		for (i = 0; i < capacity; i++)
		{
			SAFE(read_string(&mask, f));
			SAFE(read_string(&by, f));
			SAFE(read_string(&reason, f));
			SAFE(read_int32(&seton, f));
			SAFE(read_int32(&expires, f));
			fs << "\t\t<szline>" << std::endl;
			fs << "\t\t\t<mask>" << html_entities(mask) << "</mask>" << std::endl;
			fs << "\t\t\t<by>" << html_entities(by) << "</by>" << std::endl;
			fs << "\t\t\t<seton>" << seton << "</seton>" << std::endl;
			fs << "\t\t\t<expires>" << expires << "</expires>" << std::endl;
			fs << "\t\t\t<reason>" << html_entities(reason) << "</reason>" << std::endl;
			fs << "\t\t</szline>" << std::endl;
			free(mask); free(by); free(reason);
		}
		fs << "\t</szlines>" << std::endl;
		close_db(f);
	} // operserv database

	fs << "</anope>" << std::endl;

	/* CONVERTING DONE \o/ HURRAY! */
	fs.close();
	return 0;
} /* End of main() */
