Kiadott valtozatok:
  RDB_Diplomaterv_ALPHA_DEMO.hex
    - A nyom�gomb lefuto elere elinditja a motort szabalyozott +2500 RPM fordulatszammal, ujabb lefuto elre leallitja azt.
    - Hibakezeles nincs, a reset gombbal lehet ujrainditani a firmware-t.

  RDB_Diplomaterv_BETA_DEMO.hex
    - A nyom�gomb lefuto elere elinditja a tesztfolyamatot, ami a szabalyozott +2500 RPM, -2500 RPM, 0 RPM lepeseket hajtja vegre.
    - Hibakezeles nincs, a reset gombbal lehet ujrainditani a firmware-t.

  RDB_Diplomaterv_GAMMA_DEMO.hex
    - A nyom�gomb lefuto elere elinditja a tesztfolyamatot, ami a szabalyozott +2500 RPM, -2500 RPM, 0 RPM lepeseket hajtja vegre.
    - Hibakezeles nincs, a reset gombbal lehet ujrainditani a firmware-t.
    - Ha a programinditas soran a nyomogombot lenyomva tartjuk akkor az egyedi USART modban indul el a program:
	- A gyari monitorprogrammal nem kompatibilis mukodes. (A gyari monitorprogramot normal uzemmodban hasznalhatjuk.)
	- A program indulasakor a virtualis soros porton udvozlouzenetet kuld a program.
	- A tesztfolyamat elott a mert ertekek nevei felsorolasra kerulnek.
	- A tesztfolyamat alatt a mert ertekek kikuldesre kerulnek.
	- Az egyedi USART modbol a reset gombbal lephetunk ki, amennyiben az ujraindulas soran a nyomogombot nem nyomjuk le.
	- USART beallitasok: Baudrate: 115200, 8 adatbit, 1 stopbit, paritas nincs

  RDB_Diplomaterv_DELTA_DEMO.hex
    - A nyom�gomb lefuto elere elinditja a tesztfolyamatot, ami a szabalyozott +2500 RPM, -2500 RPM, 0 RPM lepeseket hajtja vegre.
    - Hibakezeles nincs, a reset gombbal lehet ujrainditani a firmware-t.
    - Ha a programinditas soran a nyomogombot lenyomva tartjuk akkor az egyedi USART modban indul el a program:
	- A gyari monitorprogrammal nem kompatibilis mukodes. (A gyari monitorprogramot normal uzemmodban hasznalhatjuk.)
	- A program indulasakor a virtualis soros porton udvozlouzenetet kuld a program.
	- A tesztfolyamat elott egy specialis "<<<START>>>" uzenet kerul kikuldesre.
	- Ezutan a mert ertekek nevei felsorolasra kerulnek.
	- A tesztfolyamat alatt a mert ertekek kikuldesre kerulnek.
	- A folyamat vegen egy specialis "<<<END>>>" uzenet kerul kikuldesre.
	- Az egyedi USART modbol a reset gombbal lephetunk ki, amennyiben az ujraindulas soran a nyomogombot nem nyomjuk le.
	- USART beallitasok: Baudrate: 115200, 8 adatbit, 1 stopbit, paritas nincs

A fejleszt�k�rnyezet haszn�lata:
  NetBeans:
    A NetBeans projekt csak edit�l�sra j�, itt ford�tani nem tudsz
  
  Keil:
    A Keil projektben tudsz ford�tani, de az edit�l�s macer�sabb
    A projekt megnyit�sa:
      Nyisd meg a "...\RDB_Diplomaterv\Software\Source\BaseProgram\STM32 PMSM FOC LIB\Web\Project\MDK-ARM\STM32F30x_Workspace.uvmpw" workspace-t.
      (�rdemes a Keil-t rendszergazdak�nt futtatni, n�ha nem tud l�trehozni f�jlokat en�lk�l)
    Ford�t�s:
      A Keil-ben a target leg�rd�l� men�b�l (nekem a "LOAD" gomb mellett van) v�laszd ki a "P-NUCLEO-IHM001_SINGLEDRIVE"-ot.
      Nyisd meg a "Projekt/Batch build..." men�t.
      Jel�ld ki a "STM32F302x8_Single_Drive"-ot �s a "P-NUCLEO-IHM001_SINGLEDRIVE"-ot.
      Ugyanitt nyomd meg a clean gombot a build gombot.
      Ezzel mindent �jraford�tasz a null�r�l, ami k�s�bb nem lesz sz�ks�ges, de az elej�n biztos, ami biztos...
      Ezut�n a "LOAD" gombbal lehet let�lteni a programot a panelre, aminek elvileg ugyanazt kell csin�lnia, mint ami alapb�l rajta volt.