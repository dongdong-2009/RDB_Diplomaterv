#ifndef _LOCAL_IO_H_
#define _LOCAL_IO_H_



//A prellmentesitesi ido deklaralasa. Ennyi ideig kell a nyomogomb nyers jelenek
  //valtozatlannak maradnia, hogy elfogadjuk, mint prellmentes jelet
#define LOCAL_IO_PRELL_LIMIT_MS	   (( U64 ) 10 )

//A LOCAL_IO modul globalis adatai
typedef struct
{
  bool bUserButtonRaw;
  bool bUserButtonDePrell;
  bool bUserButtonRising;
  bool bUserButtonFalling;
} S_LOCAL_IO_GLOBAL_DATA;



//A LOCAL_IO modul inicializalasa. Meg kell hivni az elso LOCAL_IO_Handler hivas elott.
void LOCAL_IO_Init(void);
//A LOCAL_IO modul kezelofuggvenye. Meg kell hivni az alkalmazasprogram minden ciklusaban.
void LOCAL_IO_Handler(void);
//A UserButton allapotan beolvasasa.
bool LOCAL_IO_ReadRawUserButton(void);


#endif
