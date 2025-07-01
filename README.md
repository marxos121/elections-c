# elections-c

Implementation of the D'Hondt method for allocating parliamentary seats, approximating the Polish parliamentary elections system.

Data is read via the standard input. The recommended way to run the program is by providing an appropriately formatted input file - e.g. "./elections.exe < wyniki2015.csv.crlf". Sample data included.

Flags:

	--minority [COMMITTEE] - indicates that the committee represents an ethnic minority and is therefore exempt from the standard 5% electoral threshold.

	--alliance [COMMITTEE1;COMMITTEE2;...] - indicates that the specified commitee(s) form a coalition. Their votes are combined but they must meet a higher electoral threshold of 8%.

	--sainte-lague - calculate the quotients by dividing by successive odd rather than natural numbers.
