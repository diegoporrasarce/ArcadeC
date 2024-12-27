#include <allegro.h>
#include <string>
#include <conio.h>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <winalleg.h>
#include <fstream>

using namespace std; // std::xxxxxxxx, se abrevia

#define ancho 1024
#define alto 740

MIDI   *musicaInicio;
MIDI   *musicaJuego;
SAMPLE *sonido_InicioJuego;
SAMPLE *sonido_InicioNivel;
SAMPLE *sonido_LadrilloRoto;
SAMPLE *sonido_RebotePelota;
SAMPLE *sonido_Revivir;
SAMPLE *sonido_VidaExtra;
SAMPLE *sonido_RebotaParedes;
SAMPLE *sonido_rebotaBase;
SAMPLE *sonido_vidaPerdida;
SAMPLE *sonido_gameover;

BITMAP *buffer;
BITMAP *logo;
BITMAP *panel;
BITMAP *recuadro;
BITMAP *fondo1;
BITMAP *fondo2;
BITMAP *fondo3;
BITMAP *fondo4;
BITMAP *fondo5;
BITMAP *gameover;
BITMAP *lad1;
BITMAP *lad2;
BITMAP *lad3;
BITMAP *lad4;
BITMAP *lad5;
BITMAP *lad6;
BITMAP *lad7;
BITMAP *ladd;
BITMAP *pelota;

BITMAP *base;
BITMAP *base2;
BITMAP *base3;
BITMAP *base4;

//declarar funciones
int inicializo();
void inicializo_pantalla();
void inicializo_sonidos();
void armo_pantalla();
void jugar();
void inicializo_nivel();
void chequeo_base();
void muestro_ladrillos();
void muevo_bola();
void chequeo_teclasSonido();
void configura_level();
void retoma_juego();
int cuentoLadrillos();
void destruyo_componentes();
void dibujaMuerte();
void grabo_archivo();
void cargo_archivo();


int retardo=100; //100 milisegundos de retardo (para hacer animaciones)
int vidas=3;
int level=1;
int score=0;
bool juegoIniciado=false;
bool fin=false;
bool nuevoNivel=false;
bool enJuego=false;
int dirY=-1; //cuanto más baja la bola, más alto es el número (empieza subiendo)
int dirX=1; //cuanto más a la derecha. más alto es el número (empieza saliendo hacia la derecha)
int velocidad=3; //3 píxeles
int velocidadInicial=3;
int fondoN=1;
bool muerte=false;
int secuenciaMuerte=1;
bool musica=true;
bool efectos=true;
bool finJuego=false;
bool existeArchivo=false;
int highScore=0;

int baseX=255;
int bolaX=295;
int bolaY=650;


int pant1[63]={1,1,1,1,1,1,1,1,1,
               2,2,2,2,2,2,2,2,2,
               3,3,3,3,3,3,3,3,3,
               4,4,4,4,4,4,4,4,4,
               5,5,5,5,5,5,5,5,5,
               6,6,6,6,6,6,6,6,6,
               7,7,7,7,7,7,7,7,7}; //cada numero es un tipo de ladrillo

int pant2[63]={1,1,1,1,0,0,0,1,1,
               1,1,2,2,2,2,2,2,2,
               1,2,2,0,3,0,3,0,2,
               2,5,5,5,0,1,0,5,5,
               5,4,4,6,6,6,6,6,4,
               4,7,3,7,3,7,3,7,3,
               7,3,3,3,3,3,3,3,3};

int mapa[63]; //63 ladrillos

int puntaIzq;
int puntaDer;

int colBola;
int filaBola;
int elemento;

int fila[]= {20,50,80,110,140,170,200}; //filas en pixeles

DATAFILE *datfile; //Clase 15
FONT *arialB; //Clase 15
FONT *arial20; //Clase 15

int main()
{
    try
      {
        if (inicializo() == 1) return 1;

        while ( !fin )
        {
           armo_pantalla();

           if ( key[KEY_ESC] ) fin=true;
           if ( key[KEY_ENTER] && juegoIniciado==false){
              jugar();
              midi_pause();
              if(efectos) play_sample(sonido_gameover,200,150,1000,0);

              if(score>highScore){
                highScore=score;
                grabo_archivo();
              }
              while(!key[KEY_ESC] && !key[KEY_ENTER]){
              }
              vidas=3;
              level=1;
              velocidad=velocidadInicial;
              score=0;
           }
         }

         destruyo_componentes();
      }
    catch (exception& e)
        {
         cout << e.what() << '\n';
        }
	return 0;
    }
END_OF_MAIN(); //C++



int inicializo()
{
    allegro_init(); //que se ejecute allegro
    install_keyboard();

    if(install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL) != 0){
        allegro_message("Error ! Inicializando sistema de sonido\n\n\n", allegro_error);
        return 1;
    }

    datfile=load_datafile("recursos.dat"); //Clase 15
    arialB=(FONT*)datfile[1].dat; //Clase 15
    arial20=(FONT*)datfile[0].dat; //Clase 15

    inicializo_pantalla();
    inicializo_sonidos();

    cargo_archivo();
    play_midi(musicaInicio,0);

    return 0;
}

void inicializo_pantalla()
{
    set_color_depth(32);
    set_gfx_mode(GFX_AUTODETECT,ancho,alto,0,0);
    buffer=create_bitmap(ancho,alto); //donde vamos a alojar las imagenes
    logo = load_bitmap("img/logo.bmp", NULL);
    panel = load_bitmap("img/panel.bmp",NULL);
    recuadro=load_bitmap("img/recuadro.bmp",NULL);
    fondo1=load_bitmap("img/fondo1.bmp",NULL);
    fondo2=load_bitmap("img/fondo2.bmp",NULL);
    fondo3=load_bitmap("img/fondo3.bmp",NULL);
    fondo4=load_bitmap("img/fondo4.bmp",NULL);
    fondo5=load_bitmap("img/fondo5.bmp",NULL);
    lad1=load_bitmap("img/ladrillo1.bmp",NULL);
    lad2=load_bitmap("img/ladrillo2.bmp",NULL);
    lad3=load_bitmap("img/ladrillo3.bmp",NULL);
    lad4=load_bitmap("img/ladrillo4.bmp",NULL);
    lad5=load_bitmap("img/ladrillo5.bmp",NULL);
    lad6=load_bitmap("img/ladrillo6.bmp",NULL);
    lad7=load_bitmap("img/ladrillo7.bmp",NULL);
    ladd=load_bitmap("img/ladrilloduro.bmp",NULL);
    gameover=load_bitmap("img/gameover.bmp",NULL);
    pelota=load_bitmap("img/pelota.bmp",NULL);
    base=load_bitmap("img/base.bmp",NULL);
    base2=load_bitmap("img/base2.bmp",NULL);
    base3=load_bitmap("img/base3.bmp",NULL);
    base4=load_bitmap("img/base4.bmp",NULL);

}

void inicializo_sonidos()
{
    set_volume(230, 209);

	musicaInicio  = load_midi("sonidos/ark.mid");
    musicaJuego   = load_midi("sonidos/Arkanoid.mid");

	sonido_InicioJuego = load_wav("sonidos/InicioJuego.wav");
    sonido_InicioNivel = load_wav("sonidos/InicioNivel.wav");
	sonido_LadrilloRoto = load_wav("sonidos/LadrilloRoto.wav");
	sonido_RebotePelota = load_wav("sonidos/RebotePelota.wav");
	sonido_VidaExtra = load_wav("sonidos/VidaExtra.wav");
	sonido_RebotaParedes = load_wav("sonidos/rebotaParedes.wav");
	sonido_rebotaBase = load_wav("sonidos/reboteBase.wav");
    sonido_vidaPerdida = load_wav("sonidos/fallo.wav");
    sonido_gameover = load_wav("sonidos/game-over.wav");

}

void armo_pantalla()
{
    clear_to_color(buffer, 0x000000);
    draw_sprite(buffer,logo,610,5);
    draw_sprite(buffer,panel,620,140);

    textprintf_ex(panel, arialB, 130, 28, makecol(0,0,0),makecol(0,0,0), "          "); //Clase 15, los primeros numeros son coordenadas x y de los numeros rojos
    textprintf_ex(panel, arialB, 130, 28, makecol(255,0,0),makecol(0,0,0), "%d", level); //Clase 15

    textprintf_ex(panel, arialB, 160, 90, makecol(0,0,0),makecol(0,0,0), "          ");//Clase 15
    textprintf_ex(panel, arialB, 160, 90, makecol(255,0,0),makecol(0,0,0), "%d", score);//Clase 15

    textprintf_ex(panel, arialB, 130, 160, makecol(0,0,0),makecol(0,0,0), "          ");//Clase 15
    textprintf_ex(panel, arialB, 130, 160, makecol(255,0,0),makecol(0,0,0), "%d", vidas);//Clase 15

    textprintf_ex(buffer, arial20, 700, 100, makecol(255,255,255),makecol(0,0,0), "Highscore : %i", highScore); //color blanco 255,255,255


    draw_sprite(buffer,recuadro,5,10);

     switch(fondoN)
       {
            case 1: draw_sprite(buffer,fondo1,11,16);
                    break;
            case 2: draw_sprite(buffer,fondo2,11,16);
                    break;
            case 3: draw_sprite(buffer,fondo3,11,16);
                    break;
            case 4: draw_sprite(buffer,fondo4,11,16);
                    break;
            case 5: draw_sprite(buffer,fondo5,11,16);
                    break;
        }

      if (!muerte) {
           draw_sprite(buffer,base,baseX,660);
        } else {
           switch(secuenciaMuerte)
           {
           case 1: draw_sprite(buffer,base2,baseX,655);
                   break;
           case 2: draw_sprite(buffer,base3,baseX,650);
                   break;
           case 3: draw_sprite(buffer,base4,baseX,640);
                   break;
           }
        }

    if(!enJuego)bolaX=baseX+50; //cuando no estoy en juego determino posicion bolaX
    circlefill(buffer, bolaX,bolaY, 10, makecol(124,250,16)); //dibujar bola donde corresponda



    muestro_ladrillos();
    if(vidas==0) draw_sprite(buffer,gameover,150,300);

    blit(buffer, screen, 0,0,0,0,ancho, alto);//manda el buffer a la pantalla
}

void jugar()
{
    try
    {
        level=1;
        finJuego=false;

        while (!key[KEY_ESC] && !finJuego){

            midi_pause();
            inicializo_nivel();

            while (!nuevoNivel && !key[KEY_ESC] && vidas>0){
               if(key[KEY_SPACE] && enJuego==false)
               {
                 if(efectos) stop_sample(sonido_InicioNivel);
                 if (musica) play_midi(musicaJuego,1);
                 enJuego=true;
               }
               chequeo_base();

               if (enJuego) {
                  muestro_ladrillos();
                  muevo_bola();
               }
               if(key[KEY_0]) { //tecla 0 borra mapa, para ver el siguiente nivel
                 for(int i=0;i<63;i++){
                   mapa[i]=0;
                 }
               }
              if(cuentoLadrillos()==0){
                level++;
                nuevoNivel=true;
                fondoN++;
                if (fondoN==6){ //porque solo tengo 5 pantallas
                  fondoN=1;
                }
                inicializo_nivel(); //nuevo nivel sin irme del while
              }
              chequeo_teclasSonido();
              armo_pantalla();

              if (vidas==0) finJuego=true;
            }
          }
        }
     catch (exception& e)
       {
        cout << e.what() << endl;
       }
}

void inicializo_nivel(){
    configura_level();
    retoma_juego();
    if(efectos) play_sample(sonido_InicioNivel,200,150,1000,0);

}

int cuentoLadrillos()
{
    for(int i=0;i<63;i++) {
     if(mapa[i]!=8 && mapa[i]>0) return 1; //vale con saber si hay un ladrillo
   }
   return 0;
}

void chequeo_base() //mover base
{
    if(key[KEY_RIGHT]) {
         if (baseX<476) baseX=baseX+velocidad; //ver el tope derecha
     }

     if(key[KEY_LEFT]) {
        if (baseX>11) baseX=baseX-velocidad; //ver el tope derecha
     }

}
void muestro_ladrillos()
{
     int x, y, col;
   int ladn=0;
   int lad;

   int fila[7]={20,50,80,110,140,170,200};
   for(int i=0;i<63;i++){
     if(mapa[i]>0){
         lad=mapa[i];
         y=fila[int(i/9)];
         col=i-(((int)(i/9))*9)+1;
         x=13+((col-1)*65);

         switch(lad)
            {
              case 1:
                  draw_sprite(buffer,lad1,x,y);
                  break;
              case 2:
                  draw_sprite(buffer,lad2,x,y);
                  break;
              case 3:
                  draw_sprite(buffer,lad3,x,y);
                  break;
              case 4:
                  draw_sprite(buffer,lad4,x,y);
                  break;
              case 5:
                  draw_sprite(buffer,lad5,x,y);
                  break;
              case 6:
                  draw_sprite(buffer,lad6,x,y);
                  break;
              case 7:
                  draw_sprite(buffer,lad7,x,y);
                  break;
              case 8:
                  draw_sprite(buffer,ladd,x,y);
                  break;
            }
      }
   }
}

void muevo_bola()
{
 puntaIzq=baseX+20;  //si la bola choca con la base en un extremo, cambia la direccion, por eso hay que diferenciar las partes de la base
 puntaDer=baseX+100;


 if(bolaY<225){
    filaBola=((int)(((bolaY-20)/30))+1); //-20 por el borde de arriba, /30 el alto de cada ladrillo y el espacio entre ellos (25+5)
    colBola=((int)(bolaX-13)/64)+1;
    elemento=(((filaBola-1)*9)+colBola)-1;
//colision con ladrillos
    if(mapa[elemento]!=0) {
          if (dirY==1) {
              dirY=-1;
          } else {
              dirY=1;
          }
          if(mapa[elemento] !=8) //Lo tengo que romper
          {
             if(efectos) play_sample(sonido_LadrilloRoto,200,150,1000,0);
             mapa[elemento]=0;
             score=score+10;
             muestro_ladrillos();
          } else {

             if(efectos) play_sample(sonido_RebotePelota,200,150,1000,0);

          }
    }
//colision con la base
 } else {
     if (bolaY>650 && dirY==1) {
         if (bolaX>=baseX && bolaX<=baseX+120) {

             if(efectos) play_sample(sonido_rebotaBase,200,150,1000,0);

             if(bolaX<=puntaIzq) dirX=-1;
             if(bolaX>=puntaDer) dirX=1;

             dirY=-1;
             return;

         } else {

             if(efectos) play_sample(sonido_vidaPerdida,200,150,1000,0);
             vidas--;
             dibujaMuerte();
             if (vidas>0) retoma_juego();
             return;
         }
     }
  }
//Colision con los bordes
  if (bolaX>580) dirX=-1;
  if (bolaX<15) dirX=1;
  if (bolaY<15) dirY=1;

  if (bolaX>580 || bolaX<15 || bolaY<15) {
     if(efectos) play_sample(sonido_RebotePelota,200,150,1000,0);
  }

  if (dirX==1) bolaX=bolaX+velocidad;
  if (dirX==-1) bolaX=bolaX-velocidad;
  if (dirY==1) bolaY=bolaY+velocidad;
  if (dirY==-1) bolaY=bolaY-velocidad;

  armo_pantalla();
}
void chequeo_teclasSonido()
{

     if(key[KEY_DEL]) {
       if(musica) {
          musica=false;
          midi_pause();
       } else {
          midi_resume();
          musica=true;
       }
    }

    if(key[KEY_TAB]){
        if(efectos) {
            efectos=false;
        } else {
            efectos=true;
        }
    }
}

void retoma_juego(){
    baseX=255;
    bolaX=295;
    bolaY=650;
    enJuego=false;
    nuevoNivel=false;
    armo_pantalla();
    velocidad=3+((int)level/5); //aniade dificultad
}

void configura_level(){
   for (int i = 0;i<63;i++){
      if(level==1) mapa[i]=pant1[i];
      if(level==2) mapa[i]=pant2[i];
      if(level>2)  mapa[i]= rand() % 9;
   }
}

void destruyo_componentes()
{
    destroy_midi(musicaJuego);
    destroy_sample(sonido_InicioJuego);
    destroy_sample(sonido_InicioNivel);
    destroy_sample(sonido_LadrilloRoto);
    destroy_sample(sonido_RebotePelota);
    destroy_sample(sonido_rebotaBase);
    destroy_sample(sonido_RebotaParedes);
    destroy_sample(sonido_VidaExtra);
    destroy_sample(sonido_vidaPerdida);
    destroy_sample(sonido_gameover);
	destroy_bitmap(buffer);
}

void dibujaMuerte()
{
  muerte=true;
  for (secuenciaMuerte=1;secuenciaMuerte<5;secuenciaMuerte++)
  {
     armo_pantalla();
     Sleep(200); //Detiene animacion 200 ms para poder percibirla
  }
  muerte=false;
  armo_pantalla();
}

void cargo_archivo(){
   ifstream puntaje;
   char textoPuntaje[100];

   puntaje.open("high.dat"); //para grabar nuestro highscore
   if(puntaje.fail()) {
      existeArchivo=false;
      return;
   }
   if(!puntaje.eof()) { //si no llego a fin de archivo (tiene contenido)
        puntaje.getline(textoPuntaje, sizeof(puntaje)); //redimension vector de caracteres
        string s=string(textoPuntaje); //conversion de vector a tipo string
        highScore=atoi(s.c_str()); //convertir en entero
   }
   puntaje.close();//cierro archivo

}

void grabo_archivo(){
    ofstream puntaje;

    puntaje.open("high.dat");

    puntaje << highScore << endl; //graba highScore en puntaje

    puntaje.close(); //Cerramos archivo
}

