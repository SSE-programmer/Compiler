const	string_const = 'Hello, world';
		integer_const = %10 + &10 + $14; { 2 + 8 + 20 = 30 }

Var integer1, integer2: integer;
    string1: string[10];
	string2: string;
	array1: array [2..27] of integer;

begin
		writeln('Integer2 > Integer1');
		writeln('Integer2 > Integer1');
		writeln(integer_const);
		writeln(string_const);
		readln(integer1, integer2);
		writeln(integer1, integer2);

		readln(string1);
		writeln(string1);
        readln(string2[10]);
		writeln(string2[10]);

		readln(array1[2], array1[10 + 5]);
		writeln(array1[- 4 + 6], array1[15]);

       integer1 := 100;
	   integer2 := 50;

	   string1 := 'Name';

       if integer2 <= integer1 then                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               
        begin
                writeln('Integer2 <= Integer1');
        end
        else
        begin     
                        writeln('Integer2 > Integer1');
        end;

        while integer2 and 100 do
		begin
			integer2 := integer2 div 2 mod 7 * 3 - 5; 
			writeln('Integer2 = ', integer2);
		end;
end.