const N = 15;
var
        a: array [1..15] of integer;
        i, min: integer;

begin
        i := 1;
        while i <= N do
        begin
                a[i] := i * 2;
                i := i + 1;
        end;

        a[N - 3] := -22;

        min := 1; 
        i := 1;
        while i <= N do
        begin
                if a[i] < a[min] then
                begin
                        min := i;
                end;
                i := i + 1;
        end;
        writeln(a[min]);
end.
