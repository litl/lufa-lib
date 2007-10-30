/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

/*
	Helper application, written in Java to be platform-independant.
	
	This application will extract method signatures (function
	prototypes) from C source code, and print them to the console.
	
	Nested function signatures are ignored.
	
	Usage: java MethodSig {C Source or Header File Names}
*/

import java.io.File;
import java.io.FileReader;
import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.util.ArrayList;

public class MethodSig
{
	private static ArrayList<String> DoneFiles = new ArrayList<String>();

	public static void main (String[] args)
	{
		ArrayList<String> IncludeFiles = new ArrayList<String>();
		ArrayList<String> MethodSigs   = new ArrayList<String>();
		BufferedReader    SourceFileReader;

		for (String FileName : args)
		{
			boolean DoneFile = false;
		
			if ((FileName.indexOf(".c") == -1) && (FileName.indexOf(".h") == -1))
				continue;

			for (String DoneFileName : DoneFiles)
			{
				try
				{
					if (DoneFileName.compareTo(new File(FileName).getCanonicalPath()) == 0)
						DoneFile = true;
				}
				catch (Exception e)
				{
				}
			}
			
			if (DoneFile == true)
			{
				continue;
			}
			else
			{
				try
				{
					DoneFiles.add(new File(FileName).getCanonicalPath());
				}
				catch (Exception e)
				{
				}
			}

			try
			{
				SourceFileReader  = new BufferedReader(new FileReader(FileName));
				int InBrace       = 0;
				boolean InComment = false;
				int PrevBracePos;
				
				while (SourceFileReader.ready())
				{
					String NextLine = SourceFileReader.readLine();
					
					while ((NextLine.trim().length() != 0) && 
					       (NextLine.indexOf("\\") == (NextLine.length() - 1)) &&
					       (SourceFileReader.ready()))
					{
						NextLine = NextLine.substring(0, NextLine.length() - 1)
                                 + SourceFileReader.readLine();
					}
					
					if (NextLine.indexOf("#include \"") != -1)
					{
						int    StartFileName   = NextLine.indexOf("\"");
												
						String IncludeFileName = NextLine.substring(
						                          StartFileName + 1,
												  NextLine.indexOf("\"", StartFileName + 1));

						if (FileName.lastIndexOf("\\") != -1)
							IncludeFileName = FileName.substring(0, FileName.lastIndexOf("\\") + 1)
							 + IncludeFileName;
						else if (FileName.lastIndexOf("/") != -1)
							IncludeFileName = FileName.substring(0, FileName.lastIndexOf("/") + 1) + IncludeFileName;
												  
						for (String AddedName : IncludeFiles)
						{
							if (AddedName.compareTo(IncludeFileName) == 0)
								continue;
						}
						
						try
						{
							IncludeFiles.add(new File(IncludeFileName).getCanonicalPath());
						}
						catch (Exception e)
						{
						}
						
						continue;
					}

					PrevBracePos = NextLine.indexOf("{");
					if (PrevBracePos != -1)
					{
						InBrace++;
					
						while (NextLine.indexOf("{", PrevBracePos + 1) != -1)
						{
							InBrace++;
							PrevBracePos = NextLine.indexOf("{", PrevBracePos + 1);
						}
					}
					
					PrevBracePos = NextLine.indexOf("}");
					if (PrevBracePos != -1)
					{
						InBrace--;
					
						while (NextLine.indexOf("}", PrevBracePos + 1) != -1)
						{
							InBrace--;
							PrevBracePos = NextLine.indexOf("}", PrevBracePos + 1);
						}
					}
					
					if (NextLine.indexOf("//") != -1)
						NextLine = NextLine.substring(0, NextLine.indexOf("//"));

					if (NextLine.indexOf("/*") != -1)
						InComment = true;
					
					if (NextLine.indexOf("*/") != -1)
						InComment = false;

					if ((InBrace == 0) && (InComment == false) &&
					    (NextLine.indexOf("#") == -1) && (NextLine.indexOf(";") == -1))
					{
						if ((NextLine.indexOf("(") != -1) && (NextLine.indexOf(")") != -1))
							MethodSigs.add(NextLine.trim());
					}
				}
				
				SourceFileReader.close();
			}
			catch (FileNotFoundException e)
			{
				System.out.println("Cannot open source file \"" + FileName + "\"!");
				return;
			}
			catch (Exception e)
			{
				e.printStackTrace();
			}
		}

		String[] RecFileNames = new String[IncludeFiles.size()];

		for (int i = 0; i < IncludeFiles.size(); i++)
			RecFileNames[i] = IncludeFiles.get(i);

		if (IncludeFiles.size() > 0)
			main(RecFileNames);

		for (String Sig : MethodSigs)
			System.out.println(Sig);
	}
}
