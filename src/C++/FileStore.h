/* -*- C++ -*- */

/****************************************************************************
** Copyright (c) 2001-2014
**
** This file is part of the QuickFIX FIX Engine
**
** This file may be distributed under the terms of the quickfixengine.org
** license as defined by quickfixengine.org and appearing in the file
** LICENSE included in the packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.quickfixengine.org/LICENSE for licensing information.
**
** Contact ask@quickfixengine.org if any conditions of this licensing are
** not clear to you.
**
****************************************************************************/

#ifndef FIX_FILESTORE_H
#define FIX_FILESTORE_H

#ifdef _MSC_VER
#pragma warning(disable : 4503 4355 4786 4290)
#endif

#include "MessageStore.h"
#include "SessionSettings.h"
#include <fstream>
#include <string>

namespace FIX {
class Session;

/// Creates a file based implementation of MessageStore.
class FileStoreFactory : public MessageStoreFactory {
public:
  FileStoreFactory(const SessionSettings &settings)
      : m_settings(settings) {};
  FileStoreFactory(const std::string &path)
      : m_path(path) {};

  MessageStore *create(const UtcTimeStamp &, const SessionID &);
  void destroy(MessageStore *);

private:
  std::string m_path;
  SessionSettings m_settings;
};
/*! @} */

/**
 * File based implementation of MessageStore.
 *
 * Four files are created by this implementation.  One for storing outgoing
 * messages, one for indexing message locations, one for storing sequence numbers,
 * and one for storing the session creation time.
 *
 * The formats of the files are:<br>
 * &nbsp;&nbsp;
 *   [path]+[BeginString]-[SenderCompID]-[TargetCompID].body<br>
 * &nbsp;&nbsp;
 *   [path]+[BeginString]-[SenderCompID]-[TargetCompID].header<br>
 * &nbsp;&nbsp;
 *   [path]+[BeginString]-[SenderCompID]-[TargetCompID].seqnums<br>
 * &nbsp;&nbsp;
 *   [path]+[BeginString]-[SenderCompID]-[TargetCompID].session<br>
 *
 *
 * The messages file is a pure stream of %FIX messages.<br><br>
 * The sequence number file is in the format of<br>
 * &nbsp;&nbsp;
 *   [SenderMsgSeqNum] : [TargetMsgSeqNum]<br><br>
 * The session file is a UTC timestamp in the format of<br>
 * &nbsp;&nbsp;
 *   YYYYMMDD-HH:MM:SS
 */
class FileStore : public MessageStore {
public:
  FileStore(const UtcTimeStamp &now, std::string, const SessionID &sessionID);
  virtual ~FileStore();

  bool set(SEQNUM, const std::string &) EXCEPT(IOException);
  void get(SEQNUM, SEQNUM, std::vector<std::string> &) const EXCEPT(IOException);

  SEQNUM getNextSenderMsgSeqNum() const EXCEPT(IOException);
  SEQNUM getNextTargetMsgSeqNum() const EXCEPT(IOException);
  void setNextSenderMsgSeqNum(SEQNUM value) EXCEPT(IOException);
  void setNextTargetMsgSeqNum(SEQNUM value) EXCEPT(IOException);
  void incrNextSenderMsgSeqNum() EXCEPT(IOException);
  void incrNextTargetMsgSeqNum() EXCEPT(IOException);

  UtcTimeStamp getCreationTime() const EXCEPT(IOException);

  void reset(const UtcTimeStamp &now) EXCEPT(IOException);
  void refresh() EXCEPT(IOException);

private:
#ifdef _MSC_VER
  typedef std::pair<long, std::size_t> OffsetSize;
#else
  typedef std::pair<long, std::size_t> OffsetSize;
#endif
  typedef std::map<SEQNUM, OffsetSize> NumToOffset;

  void open(bool deleteFile);
  void populateCache();
  bool readFromFile(int offset, int size, std::string &msg);
  void setSeqNum();
  void setSession();

  bool get(SEQNUM, std::string &) const EXCEPT(IOException);

  MemoryStore m_cache;
  NumToOffset m_offsets;

  std::string m_msgFileName;
  std::string m_headerFileName;
  std::string m_seqNumsFileName;
  std::string m_sessionFileName;

  FILE *m_msgFile;
  FILE *m_headerFile;
  FILE *m_seqNumsFile;
  FILE *m_sessionFile;
};
} // namespace FIX

#endif // FIX_FILESTORE_H
