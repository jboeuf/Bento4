/*****************************************************************
|
|    AP4 - OMA DCF support
|
|    Copyright 2002-2006 Gilles Boccon-Gibod & Julien Boeuf & Julien Boeuf
|
|
|    This file is part of Bento4/AP4 (MP4 Atom Processing Library).
|
|    Unless you have obtained Bento4 under a difference license,
|    this version of Bento4 is Bento4|GPL.
|    Bento4|GPL is free software; you can redistribute it and/or modify
|    it under the terms of the GNU General Public License as published by
|    the Free Software Foundation; either version 2, or (at your option)
|    any later version.
|
|    Bento4|GPL is distributed in the hope that it will be useful,
|    but WITHOUT ANY WARRANTY; without even the implied warranty of
|    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
|    GNU General Public License for more details.
|
|    You should have received a copy of the GNU General Public License
|    along with Bento4|GPL; see the file COPYING.  If not, write to the
|    Free Software Foundation, 59 Temple Place - Suite 330, Boston, MA
|    02111-1307, USA.
|
****************************************************************/

#ifndef _AP4_OMA_DCF_H_
#define _AP4_OMA_DCF_H_

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include "Ap4Types.h"
#include "Ap4SampleEntry.h"
#include "Ap4Atom.h"
#include "Ap4AtomFactory.h"
#include "Ap4SampleDescription.h"
#include "Ap4Processor.h"
#include "Ap4Protection.h"

/*----------------------------------------------------------------------
|   class references
+---------------------------------------------------------------------*/
class AP4_CtrStreamCipher;
class AP4_OdafAtom;
class AP4_CbcStreamCipher;
class AP4_CtrStreamCipher;

/*----------------------------------------------------------------------
|   constants
+---------------------------------------------------------------------*/
const AP4_UI32 AP4_PROTECTION_SCHEME_TYPE_OMA = AP4_ATOM_TYPE('o','d','k','m');
const AP4_UI32 AP4_PROTECTION_SCHEME_VERSION_OMA_20 = 0x00000200;

/*----------------------------------------------------------------------
|   AP4_OmaDcfSampleDecrypter
+---------------------------------------------------------------------*/
class AP4_OmaDcfSampleDecrypter : public AP4_SampleDecrypter
{
public:
    // factory
    static AP4_OmaDcfSampleDecrypter* Create(AP4_ProtectedSampleDescription* sample_description, 
                                             const AP4_UI08* key, AP4_Size key_size);

    // constructor and destructor
    AP4_OmaDcfSampleDecrypter(AP4_Size iv_length) : m_IvLength(iv_length) {}

    // methods
    virtual AP4_Size GetDecryptedSampleSize(AP4_Sample& sample) = 0;

protected:
    AP4_Size m_IvLength;
};

/*----------------------------------------------------------------------
|   AP4_OmaDcfCtrSampleDecrypter
+---------------------------------------------------------------------*/
class AP4_OmaDcfCtrSampleDecrypter : public AP4_OmaDcfSampleDecrypter
{
public:
    // constructor and destructor
    AP4_OmaDcfCtrSampleDecrypter(const AP4_UI08* key);
    ~AP4_OmaDcfCtrSampleDecrypter();

    // methods
    virtual AP4_Result DecryptSampleData(AP4_DataBuffer& data_in,
                                         AP4_DataBuffer& data_out);
    virtual AP4_Size   GetDecryptedSampleSize(AP4_Sample& sample);

private:
    // members
    AP4_CtrStreamCipher* m_Cipher;
};

/*----------------------------------------------------------------------
|   AP4_OmaDcfCbcSampleDecrypter
+---------------------------------------------------------------------*/
class AP4_OmaDcfCbcSampleDecrypter : public AP4_OmaDcfSampleDecrypter
{
public:
    // constructor and destructor
    AP4_OmaDcfCbcSampleDecrypter(const AP4_UI08* key);
    ~AP4_OmaDcfCbcSampleDecrypter();

    // methods
    virtual AP4_Result DecryptSampleData(AP4_DataBuffer& data_in,
                                         AP4_DataBuffer& data_out);
    virtual AP4_Size   GetDecryptedSampleSize(AP4_Sample& sample);

private:
    // members
    AP4_CbcStreamCipher* m_Cipher;
};

/*----------------------------------------------------------------------
|   AP4_OmaDcfTrackDecrypter
+---------------------------------------------------------------------*/
class AP4_OmaDcfTrackDecrypter : public AP4_Processor::TrackHandler {
public:
    // constructor
    static AP4_OmaDcfTrackDecrypter* Create(const AP4_UI08*                 key,
                                            AP4_ProtectedSampleDescription* sample_description,
                                            AP4_SampleEntry*                sample_entry);
    virtual ~AP4_OmaDcfTrackDecrypter();

    // methods
    virtual AP4_Size   GetProcessedSampleSize(AP4_Sample& sample);
    virtual AP4_Result ProcessTrack();
    virtual AP4_Result ProcessSample(AP4_DataBuffer& data_in,
                                     AP4_DataBuffer& data_out);

private:
    // constructor
    AP4_OmaDcfTrackDecrypter(AP4_OmaDcfSampleDecrypter* cipher,
                             AP4_SampleEntry*           sample_entry,
                             AP4_UI32                   original_format);

    // members
    AP4_OmaDcfSampleDecrypter* m_Cipher;
    AP4_SampleEntry*           m_SampleEntry;
    AP4_UI32                   m_OriginalFormat;
};

/*----------------------------------------------------------------------
|   AP4_OmaDcfSampleEncrypter
+---------------------------------------------------------------------*/
class AP4_OmaDcfSampleEncrypter
{
public:
    // constructor and destructor
    AP4_OmaDcfSampleEncrypter(const AP4_UI08* salt);
    virtual ~AP4_OmaDcfSampleEncrypter() {}

    // methods
    virtual AP4_Result EncryptSampleData(AP4_DataBuffer& data_in,
                                         AP4_DataBuffer& data_out,
                                         AP4_UI32        bso,
                                         bool            skip_encryption) = 0;
    virtual AP4_Size   GetEncryptedSampleSize(AP4_Sample& sample) = 0;

protected:
    // members
    AP4_UI08 m_Salt[16];
};

/*----------------------------------------------------------------------
|   AP4_OmaDcfCtrSampleEncrypter
+---------------------------------------------------------------------*/
class AP4_OmaDcfCtrSampleEncrypter : public AP4_OmaDcfSampleEncrypter
{
public:
    // constructor and destructor
    AP4_OmaDcfCtrSampleEncrypter(const AP4_UI08* key,
                                 const AP4_UI08* salt);
    ~AP4_OmaDcfCtrSampleEncrypter();

    // methods
    virtual AP4_Result EncryptSampleData(AP4_DataBuffer& data_in,
                                         AP4_DataBuffer& data_out,
                                         AP4_UI32        bso,
                                         bool            skip_encryption);
    virtual AP4_Size   GetEncryptedSampleSize(AP4_Sample& sample);

private:
    // members
    AP4_CtrStreamCipher* m_Cipher;
};

/*----------------------------------------------------------------------
|   AP4_OmaDcfCbcSampleEncrypter
+---------------------------------------------------------------------*/
class AP4_OmaDcfCbcSampleEncrypter : public AP4_OmaDcfSampleEncrypter
{
public:
    // constructor and destructor
    AP4_OmaDcfCbcSampleEncrypter(const AP4_UI08* key,
                                 const AP4_UI08* salt);
    ~AP4_OmaDcfCbcSampleEncrypter();

    // methods
    virtual AP4_Result EncryptSampleData(AP4_DataBuffer& data_in,
                                         AP4_DataBuffer& data_out,
                                         AP4_UI32        bso,
                                         bool            skip_encryption);
    virtual AP4_Size   GetEncryptedSampleSize(AP4_Sample& sample);

private:
    // members
    AP4_CbcStreamCipher* m_Cipher;
};

/*----------------------------------------------------------------------
|   AP4_TrackPropertyMap
+---------------------------------------------------------------------*/
class AP4_TrackPropertyMap
{
public:
    // methods
    AP4_Result  SetProperty(AP4_UI32 track_id, const char* name, const char* value);
    AP4_Result  SetProperties(const AP4_TrackPropertyMap& properties);
    const char* GetProperty(AP4_UI32 track_id, const char* name);

    // destructor
    virtual ~AP4_TrackPropertyMap();

private:
    // types
    class Entry {
    public:
        Entry(AP4_UI32 track_id, const char* name, const char* value) :
          m_TrackId(track_id), m_Name(name), m_Value(value) {}
        AP4_UI32   m_TrackId;
        AP4_String m_Name;
        AP4_String m_Value;
    };

    // members
    AP4_List<Entry> m_Entries;
};

/*----------------------------------------------------------------------
|   AP4_OmaDcfEncryptingProcessor
+---------------------------------------------------------------------*/
class AP4_OmaDcfEncryptingProcessor : public AP4_Processor
{
public:
    // accessors
    AP4_ProtectionKeyMap& GetKeyMap()      { return m_KeyMap;      }
    AP4_TrackPropertyMap& GetPropertyMap() { return m_PropertyMap; }

    // methods
    virtual AP4_Processor::TrackHandler* CreateTrackHandler(AP4_TrakAtom* trak);

private:
    // members
    AP4_ProtectionKeyMap m_KeyMap;
    AP4_TrackPropertyMap m_PropertyMap;
};

#endif // _AP4_OMA_DCF_H_
