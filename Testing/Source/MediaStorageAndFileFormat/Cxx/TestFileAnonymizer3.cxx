/*=========================================================================

  Program: GDCM (Grassroots DICOM). A DICOM library

  Copyright (c) 2006-2011 Mathieu Malaterre
  All rights reserved.
  See Copyright.txt or http://gdcm.sourceforge.net/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "gdcmFileAnonymizer.h"

#include "gdcmTesting.h"
#include "gdcmSystem.h"
#include "gdcmReader.h"

#include <vector>

static int TestFileAnonymize3(const char *filename, bool verbose = false)
{
  using namespace gdcm;
  if( verbose )
    std::cout << "Processing: " << filename << std::endl;

  // Create directory first:
  const char subdir[] = "TestFileAnonymize3";
  std::string tmpdir = Testing::GetTempDirectory( subdir );
  if( !System::FileIsDirectory( tmpdir.c_str() ) )
    {
    System::MakeDirectory( tmpdir.c_str() );
    //return 1;
    }
  std::string outfilename = Testing::GetTempFilename( filename, subdir );

  gdcm::Tag t1(0x0018,0x5100);
  gdcm::Tag t2(0x0018,0x1312);
  gdcm::Tag t3(0x0018,0x1313);
  gdcm::Tag t4(0x0018,0x1317);
  //gdcm::Tag t5(0x0008,0x2112); // SQ
  //gdcm::Tag t6(0x0008,0x9215); // SQ
  gdcm::Tag t7(0x0018,0x1020);
  gdcm::Tag t8(0x0004,0x1130); // Test DICOMDIR !
  gdcm::Tag t9(0x0008,0x0000); // Earliest possible Data Element
  gdcm::Tag t0(0xffff,0xffff); // Latest Possible Element

  std::vector<gdcm::Tag> tags;
  tags.push_back( t0 );
  tags.push_back( t1 );
  tags.push_back( t2 );
  tags.push_back( t3 );
  tags.push_back( t4 );
  tags.push_back( t7 );
  tags.push_back( t8 );
  tags.push_back( t9 );

  gdcm::FileAnonymizer fa;
  fa.SetInputFileName( filename );
  fa.SetOutputFileName( outfilename.c_str() );
  for( std::vector<gdcm::Tag>::const_iterator it = tags.begin();
    it != tags.end(); ++it )
    {
    fa.Replace( *it, "TOTO" );
    }
  if( !fa.Write() )
    {
    std::cerr << "Failed to write: " << outfilename << std::endl;
    return 1;
    }

  // Read back and check:
  gdcm::Reader r;
  r.SetFileName( outfilename.c_str() );
  if( !r.Read() )
    {
    return 1;
    }
  const File &f = r.GetFile();
  const DataSet &ds = f.GetDataSet();
  for( std::vector<gdcm::Tag>::const_iterator it = tags.begin();
    it != tags.end(); ++it )
    {
    const gdcm::Tag & t = *it;
    if( !ds.FindDataElement( t ) )
      {
      return 1;
      }
    const gdcm::DataElement & de = ds.GetDataElement( t );
    if( de.GetVL() != 4 )
      {
      return 1;
      }
    }


  return 0;
}

int TestFileAnonymizer3(int argc, char *argv[])
{
  if( argc == 2 )
    {
    const char *filename = argv[1];
    return TestFileAnonymize3(filename, true);
    }

  // else
  gdcm::Trace::DebugOff();
  gdcm::Trace::WarningOff();
  gdcm::Trace::ErrorOff();
  int r = 0, i = 0;
  const char *filename;
  const char * const *filenames = gdcm::Testing::GetFileNames();
  while( (filename = filenames[i]) )
    {
    r += TestFileAnonymize3( filename );
    ++i;
    }

  return r;
}
