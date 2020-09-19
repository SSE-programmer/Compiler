var     str, s: string;
        i, j, s1, s2: integer;
        flag: integer;

begin
        writeln('Input first string: ');
        readln(str);
        writeln('Input first string length: ');
        readln(s1);
        writeln('Input second string: ');
        readln(s);
        writeln('Input second string length: ');
        readln(s2);

        if s2 > s1 then
        begin 
                writeln('ОШИБКА: Вторая строка длинее первой!');
        end;

        i := 1;
        flag := 0;
        while i <= s1 do
        begin
                if str[i] = s[1] then
                begin
                        flag := 1;
                        j := 1;
                        while (i + j <= s1) and (j + 1 <= s2) do
                        begin
                                if str[i + j] = s[j + 1] then
                                begin
                                        writeln(s[j], i + j - 1);
                                        flag := 1;
                                end
                                else
                                begin
                                        flag := 0;
                                end;

                                j := j + 1;
                        end;
                        if (flag = 1) and (j >= s2) then
                        begin
                                writeln('Second string find in first string: ', i);
                        end;
                end;
                i := i + 1;
        end;
end.
