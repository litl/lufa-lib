/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

/*
	Helper application, written in Java to be platform-independant.
	
	This application will determine any unhooked events in the user
	application, and print them in a formatted list to the console
	output. It requires the path of the MyUSB events header file, the
	path of the target application's MAP file, and the path of the
	MyUSB events object file.
	
	Usage: java CheckLibEventHooks -E {MyUSB Events Header File Path}
	                               -M {User Application MAP file}
								   -P {MyUSB Events Object File Path}
	
	This application may be added to the target application's makefile, for
	automated checking at build time. An example makefile target is as follows:

	** SAMPLE START **
	|checkhooks:
	|	@java -classpath MyUSB/HelperApps/CheckLibEventHooks/ CheckLibEventHooks \
	|		-E MyUSB/Drivers/USB/HighLevel/Events.h                              \
	|       -M $(TARGET).map                                                     \
	|		-P MyUSB/Drivers/USB/HighLevel/Events.o
	** SAMPLE END **
*/

import java.io.FileReader;
import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.util.ArrayList;

public class CheckLibEventHooks
{
	private static String EventSearchString = "ALIAS_STUB";

	public static void main (String[] args)
	{
		String            EventsFileName = GetArgString("-E", args);
		String            MapFileName    = GetArgString("-M", args);
		String            EventsFilePath = GetArgString("-P", args);
		boolean           UnhookedEvents = false;
		ArrayList<String> EventNames     = new ArrayList<String>();
		BufferedReader    EventsFileReader;
		BufferedReader    MapFileReader;
	
		if (EventsFileName == null)
		{
			System.out.println("No Events header file specified (-E switch)");
			return;
		}
			
		if (MapFileName == null)
		{
			System.out.println("No map file specified (-M switch)");
			return;
		}
		
		if (EventsFilePath == null)
		{
			System.out.println("No events object file path specified (-P switch)");
			return;		
		}
		
		try
		{
			EventsFileReader = new BufferedReader(new FileReader(EventsFileName));

			while (EventsFileReader.ready())
			{
				String NextLine = EventsFileReader.readLine();
				
				boolean IsEvent   = (NextLine.indexOf(EventSearchString) != -1);
				int     NameStart = NextLine.indexOf("(") + 1;
				int     NameEnd = NextLine.indexOf(")", NameStart);
				
				if (IsEvent && (NameStart != (NameEnd - 1)))
					EventNames.add(NextLine.substring(NameStart, NameEnd));
			}

			EventsFileReader.close();
		}
		catch (FileNotFoundException e)
		{
			System.out.println("Cannot open events header file!");
			return;
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
		
		try
		{
			MapFileReader = new BufferedReader(new FileReader(MapFileName));

			System.out.println("\n--- Unhooked MyUSB Events: ---");

			while (MapFileReader.ready())
			{
				String NextLine = MapFileReader.readLine();
				
				for (String EventName : EventNames)
				{
					if ((NextLine.indexOf(EventName) != -1) && (NextLine.indexOf(".o") != -1))
					{
						if (NextLine.indexOf(EventsFilePath) != -1)
						{
							System.out.println("  " + EventName);
							UnhookedEvents = true;
						}

						break;
					}
				}
			}

			if (UnhookedEvents == false)
				System.out.println("  (None)");
			
			System.out.println("------------------------------");

			MapFileReader.close();
		}
		catch (FileNotFoundException e)
		{
			System.out.println("Cannot open map file!");
			return;
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}		
	}

	public static String GetArgString (String Switch, String[] args)
	{
		for (int a = 0; a < args.length; a++)
		{
			String Arg = args[a];
		
			if (Arg.indexOf(Switch) != -1)
			{
				if (Arg.length() > Switch.length())
					return Arg.substring(Switch.length());
				else
					return args[a + 1];
			}
		}
		
		return null;
	}
}
