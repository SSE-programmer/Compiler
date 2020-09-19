var k, l, n: integer;


begin
        writeln('Input two numbers: ');
        readln(k, l);

        while (k <> 0) and (l <> 0) do
        begin
                if k >= l then
                begin
                        k := k mod l;
                end
                else
                begin
                        l := l mod k;
                end;
        end;

        n := k + l;

        writeln('NOD = ', n);
end.
