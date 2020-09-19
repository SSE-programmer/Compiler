const s = 'wewgweg';

var i: array [3..10] of integer;
	str: string [20];
	p: integer;

begin
	p := 5;
	
	str := 'Hello world';
	str[5] := 'c';

	writeln(s);


	if 'a' = 'b' then
	begin
		writeln(str, 1);
	end
	else
	begin
		writeln(0);
	end; 

end.