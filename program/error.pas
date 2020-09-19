const d = 
        writeln('Input first string: ');
        readln(str);
        writeln('Input second string: ');
        readln(s);
        i := 1;
        while i <= length(str) do
        begin
                if str[i] = s[1] then
                begin
                        flag := true;
                        j := 1;
                        while (i + j <= length(str)) and (j + 1 <= length(s)) do
                        begin
                                if str[i + j] <> s[j + 1] then
                                        flag := false;
                                j := j + 1;
                        end;
                        if (flag = true) and (j >= length(s)) then
                                writeln('Second string find in first string: ', i);
                end;
                i := i + 1;
        end;
readln;
end.
