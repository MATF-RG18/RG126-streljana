#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>

#define PI 3.14
#define TIMER_ID 0
#define TIMER_INTERVAL 20
#define NUMBER_OF_DOTS 10000

/*rastojanje od nisana do mete*/
#define DISTANCE_TAGET -64

/* Deklaracije callback funkcija. */
static void on_display(void);
static void on_keyboard(unsigned char key, int x, int y);
static void on_reshape(int width, int height);
static void on_timer(int value);
static void on_mouse(int button, int state, int x, int y);
static void on_motion(int x, int y);
void textFunc(const char* text, double x, double y);

/*metod koji crta nisan*/
static void  draw_gunsight(void);

/*flag za pogled*/
static int view_flag=0;

/* Vreme proteklo od pocetka simulacije. */
static int animation_ongoing;
static float animation_parameter;

/*globalne promenjive za polozaj misa u ekranu */
static double crosshairX;
static double crosshairY;
static int heightW;
static double widthW;

/*globalne promenjive za polozaj nisana u ekranu*/
GLdouble gunX = 0;
GLdouble gunY = 0;

/*flag za opaljeni metak*/
static int fired_flag=0;

/*globalne promenjive za polozaj u kom je opaljen metak*/
GLdouble buletX = 0;
GLdouble buletY = 0;

/*ispis pogotka*/
static char hitnumber[2];

/*ugao za rotaciju nisana*/
GLdouble gun_lean = 0;

int main(int argc, char *argv[])
{
	/* Inicijalizuje se GLUT. */
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	
	/* Kreira se prozor. */
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);

	/* Registruju se funkcije za obradu dogadjaja. */
	glutKeyboardFunc(on_keyboard);
	glutReshapeFunc(on_reshape);
	glutDisplayFunc(on_display);
  
    glutMouseFunc(on_mouse);	
	glutPassiveMotionFunc(on_motion);
	
	/*inicijalizacija animacije*/
	animation_ongoing = 0;
    animation_parameter = 0;


	/* Obavlja se OpenGL inicijalizacija. */
	glClearColor(0.75f,0.75f,0.75f, 0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	/* Ulazi se u glavnu petlju. */
	glutMainLoop();

	return 0;
}

/*funkcija za pracenje x i y koordinate prilikom pomeranja misa*/
static void on_motion(int x, int y){
    crosshairY = y;
    crosshairX = x;
    glutPostRedisplay();
}

/*klik misa-> ispaljen metak*/
static void on_mouse(int button, int state, int x, int y){
    switch(button) {
        case GLUT_LEFT_BUTTON:
			glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
			animation_ongoing = 1;
			
			/*ispaljeni metak -> metak ne prati vise nisan*/
			fired_flag=1;
			buletX = gunX;
			buletY = gunY;
			
			/* Forsira se ponovno iscrtavanje prozora. */
            glutPostRedisplay();
    }   
}

static void on_reshape(int width, int height) {

    widthW = width;
    heightW = height;

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(
		60, 
		(float)width/height, 
		1, 1500
		);
}

static void on_keyboard(unsigned char key, int x, int y) {
	switch(key) {
		case 27:
			/* Zavrsava se program. */
			exit(0);
			break;
		case 'g':
			/* klik na malo g-> ispaljen metak*/
			if (!animation_ongoing) {
				glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
				animation_ongoing = 1;
				
				fired_flag=1;
				buletX = gunX;
				buletY = gunY;
				
				glutPostRedisplay();
			}
			break;
		case 'P':
			/* Privremeno se zaustavlja simulacija. */
			animation_ongoing = 0;
			break;
		case 'r':
			/*restart metka*/
			animation_parameter = 0;
			animation_ongoing = 0;
			fired_flag=0;
			break;
		case 'p':
			/*menjanje pogleda*/
			//TODO zum kamere
			/*kad je 1 zum na metu*/
			if(view_flag == 0)
				view_flag=1;
			else
				view_flag=0;
			
			/* Forsira se ponovno iscrtavanje prozora. */
			glutPostRedisplay();
			break;
		
        case 'z':
        	/* Prati se mis po ekranu */
            crosshairX = x;
            crosshairY = y;
            /* Forsira se ponovno iscrtavanje prozora. */
            glutPostRedisplay();
            break;
       /*za sad se pokrece na wasd, a u narednim verzijama bice prebaceno na mis*/
       case 'w':
            gunY += 0.1;
            glutPostRedisplay();
            break;
       case 's':
            gunY -= 0.1;
            glutPostRedisplay();
            break;
       case 'a':
            gunX -= 0.1;
            glutPostRedisplay();
            break;
       case 'd':
            gunX += 0.1;
            glutPostRedisplay();
            break;
		/*krivimo nisan, tj namestam ugao za koj se krivi*/
       case 'q':
       		/*krivi levo ili ako je bio kriv desno vrati na normalu*/
            if(gun_lean == 0 || gun_lean == -15)
            	gun_lean += 15;
            glutPostRedisplay();
            break;
       case 'e':
       		/*krivi desno ili ako je bio kriv levo vrati na normalu*/
       		if(gun_lean == 0 || gun_lean == 15)
            	gun_lean -= 15;
            glutPostRedisplay();
            break;

	}

}

static void on_timer(int value) {
	/* Proverava se da li callback dolazi od odgovarajuceg tajmera. */
	if (value != TIMER_ID)
		return;
	
	/* Azurira se vreme simulacije. */
    animation_parameter += 0.2;
    /* Forsira se ponovno iscrtavanje prozora. */
	glutPostRedisplay();

	if (animation_ongoing == 1 && animation_parameter < -( DISTANCE_TAGET -0.1)) {
		glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
	} else {
		/* Zaustavlja se metak kod mete */
        animation_ongoing = 0;
        

        /*izracunavanje pogodtka*/
        //centar je (0;0.14)
        double radius= sqrt((buletX)*(buletX)+(buletY-0.14)*(buletY-0.14));
        //rastojanje izmedju krugova je 0.3, a ide od 0 do 3
        int hit =11 - ceil((radius)/0.3);
        sprintf(hitnumber, "%d", hit);
    }

}

/* Iscrtavanje scene */
static void on_display(void) {

	/* Postavlja se boja svih piksela na zadatu boju pozadine. */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* Postavlja se vidna tacka. */
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	/*pozicija kamere*/
	if(view_flag == 0){
    	/*kamera za nisanjenje */
		gluLookAt(
			0, 2, 14,
			0, 0, 0,
			0, 1, 0
			);
	}else {
		/* kamera kad je zumiranje ukljuceno */
		//TODO popraviti
		gluLookAt(
			0, 4, 10,
			0, 0, 0,
			0, 1, 0
			);
	}
	
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	/*ukljucivanje svetlosti*/
	GLfloat light_position[] = {1, 1, 1};
	GLfloat light_ambient[] = {1, 1, 1, 1};
	GLfloat light_diffuse[] = {1, 1, 1, 1};

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);


	/* krugovi na meti i okolina mete*/
	glPushMatrix();
			float r = 0.3; int j;
            int i;
            glDisable(GL_LIGHT0);
            glDisable(GL_LIGHTING);

			for (j = 0; j < 10; j++) {
                /*boja linija mete*/
                glColor3f(0,0,0);
            
            /*debljina krugova mete*/
            glLineWidth(3);
            
            /* krugovi na meti */
    		glBegin(GL_LINE_STRIP);
                double angle1 = 0.0;
                float angle = 2 * M_PI / NUMBER_OF_DOTS;
				for (i = 0; i < NUMBER_OF_DOTS; i++) {
					glVertex3f(r * cos(angle1), r * sin(angle1), DISTANCE_TAGET );
                    angle1 += angle;
				}
		    glEnd();
				r += 0.3;

			}
            glEnable(GL_LIGHT0);
            glEnable(GL_LIGHTING);
            
            glTranslatef(-5, -10, 0);
            glDisable(GL_LIGHT0);
            glDisable(GL_LIGHTING);
            
            /*boja manjeg trapeza*/
            glColor3f(1, 1, 1);
            /* koordinate temena manjeg trapeza*/
            glBegin(GL_QUADS);
	            glVertex3f(1.9, 6.9, DISTANCE_TAGET);
                glVertex3f(1.9, 13.1, DISTANCE_TAGET);
                glVertex3f(8.1, 13.1,DISTANCE_TAGET);
                glVertex3f(8.1, 6.9, DISTANCE_TAGET);
            glEnd();
            
            
            /*boja  veceg trapeza*/
            glColor3f(0.8f, 0.52f, 0.25f);
            /* koordinate temena veceg trapeza*/
            glBegin(GL_QUADS);
                glVertex3f(-4.5, 5, DISTANCE_TAGET -0.1);
                glVertex3f(-4.5, 15, DISTANCE_TAGET -0.1);
                glVertex3f(14, 15, DISTANCE_TAGET -0.1);
                glVertex3f(14, 5, DISTANCE_TAGET -0.1);

    glEnd();
	glPopMatrix();
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    
    
    /*crtanje "metka"*/
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
    glPushMatrix();
    	if(fired_flag==0){
    		/*pozicija ne ispaljenog metka*/
    		glTranslatef(0 + gunX, -1 + gunY, 4-animation_parameter);
    	}
    	else{
 	   		/*pozicija ispaljenog metka*/
    		glTranslatef(0 + buletX, -1 + buletY, 4-animation_parameter);
    	}
        glColor3f(1, 0, 0);
        glutSolidSphere(0.1, 40, 40);
    glPopMatrix();
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    
    /*crtanje nisana*/
    glTranslatef(gunX, gunY, 0);
    draw_gunsight();
	
	/*stampanje na ekran*/
	textFunc("Shoting Range", 150, 550);
	textFunc(hitnumber, 100, 100);

	/* Postavlja se nova slika u prozor. */
	glutSwapBuffers();
}

/* crtanje nisana */
static void  draw_gunsight(void){
	glPushMatrix();
		
		/*zelena boja materijala*/
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, (float[4]){ 0.0215f, 0.1745f, 0.0215f, 1.0f });
	  	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, (float[4]){0.4f, 0.5f, 0.4f, 1.0f });
	   	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, (float[4]){0.04f, 0.7f, 0.04f, 1.0f });
	   	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.78125f);
   			
#define	 glutWireCube  glutSolidCube
		
		/*krivim nisan */
		glRotatef(gun_lean, 0, 0, 1);		
		glTranslatef(0, -1, 0);

		glTranslatef(0, 0, 6);
    	glScalef(0.75, 0.6, 6);
    	glutSolidCube(1);
		//glutWireCube(1);
		
		glTranslatef(0, 0.56, 0);
    	glScalef(1.4, 0.2, 3.2);
    	glutSolidCube(0.3);
		//glutWireCube(0.3);
		
		//kuglica na koje ide zadnje musice
		glTranslatef(0, 0, 0.10);
    	glScalef( 1.01, 4, 0.1);
    	glutSolidSphere(0.22, 40, 40);
    	
    	/*zuta boja za musice*/
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, (float[4]){0.75164f, 0.60648f, 0.22648f, 1.0f});
   		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, (float[4]){0.329412f, 0.223529f, 0.027451f, 1.0f });
   		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, (float[4]){0.992157f, 0.941176f, 0.807843f, 1.0f });
   		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.21794872f);
    	
    	// n_n 
    	glTranslatef(-0.05, 0.18, 0);
    	glScalef(0.2, 2, 1);
    	//glutSolidCube(0.25);
		glutWireCube(0.25);
    	
    	glTranslatef(0.55, 0, 0);
    	//glutSolidCube(0.25);
		glutWireCube(0.25);
    	
    	// _n_ 
    	glTranslatef(-0.25, 0.01, -2.5);
    	glScalef(1.2, 0.95, 1);
    	//glutSolidCube(0.25);
		glutWireCube(0.25);
		    	
#undef glutCube    	

	glPopMatrix();
}

/*Postavljanje teksta na ekran*/
void textFunc(const char* text, double x, double y){
    glPushMatrix(); 
    
    glDisable(GL_LIGHTING); 
    glColor3f(0, 0, 0); 

    glMatrixMode(GL_PROJECTION); 
    double matrix[16];
    glGetDoublev(GL_PROJECTION_MATRIX, matrix);
    glLoadIdentity();
    glOrtho(0, widthW, 0, heightW, -5, 5);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRasterPos2f(x,y); 
    
    for(int i = 0; text[i]; i++){
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (int)text[i]); 
    }
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixd(matrix); 
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_LIGHTING); 

    glPopMatrix();
}
