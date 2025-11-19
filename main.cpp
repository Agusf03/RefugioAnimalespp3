

//
// Created by Constanza Gabriel on 10/18/25.
//
/*
#include <iostream>
#include <limits>
#include <string>

#include <cctype>
#include <vector>
#include <algorithm>
#include <locale.h>
#include <cstdlib> // Necesario para system()

// Librería para el menú interactivo
#include <ncurses.h>


// INCLUDES 
#include "Base/Animal.h"
#include "Utilidad/Enums.h"
#include "Core/Refugio.h"
#include "Dominio/Perro.h"
#include "Dominio/Gato.h"
#include "Dominio/Cuidador.h"
#include "Dominio/Adopcion.h"
#include "Dominio/Vacunacion.h"
#include "Dominio/Desparasitacion.h"

using namespace std;

// FunciOn auxiliar estAtica para evitar el error de "duplicate symbol"
static void limpiarBuffer() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// -------------------------------------------------------------
// FUNCIONES DE UTILIDAD: PAUSA ROBUSTA Y LIMPIEZA
// -------------------------------------------------------------

void pausarYLimpiar() {
    cout << "\nPresione ENTER para continuar...";

    // 1. Limpia cualquier estado de error o flag 
    if (cin.fail()) {
        cin.clear();
    }

    // 2. Limpia el buffer de entrada de cualquier caracter residual.
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    // 3. Espera por una sola pulsación de ENTER.
    cin.get();

    // 4. Limpieza forzada de la terminal (CRÍTICO)
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}


// -------------------------------------------------------------
// FUNCIONES MODULARES NCURSES
// -------------------------------------------------------------

// 1. Dibujo del menu centrado resaltando la opcion actual
void draw_menu(WINDOW *menu_win, const vector<string>& options, int highlight) {
    int max_y, max_x, i;

    getmaxyx(menu_win, max_y, max_x);
    int y_start = 2;

    box(menu_win, 0, 0);

    for (i = 0; i < options.size(); ++i) {
        int option_length = options[i].length() + 4;
        int x_start = (max_x - option_length) / 2;

        if (highlight == i) {
            wattron(menu_win, A_REVERSE);
        }

        mvwprintw(menu_win, y_start + i, x_start, "%d. %s", i + 1, options[i].c_str());

        wattroff(menu_win, A_REVERSE);
    }
    wrefresh(menu_win);
}

// 2. Muestra un recuadro de mensaje centrado (Requisito 3)
void show_message_box(const string& message) {
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    int h = 5;
    int w = message.length() + 6;
    int start_y = (max_y - h) / 2;
    int start_x = (max_x - w) / 2;

    WINDOW *message_win = newwin(h, w, start_y, start_x);
    box(message_win, 0, 0);

    mvwprintw(message_win, 1, 2, message.c_str());
    mvwprintw(message_win, 3, 2, "Presione ENTER para continuar...");

    wrefresh(message_win);

    int ch;
    while ((ch = wgetch(message_win)) != 10 && ch != '\n');

    delwin(message_win);
    endwin();

    // LIMPIEZA EXPLÍCITA DEL SISTEMA
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// 3. Ejecuta el bucle principal de navegación (Requisito 2)
int run_menu(const vector<string>& options, const string& title) {
    int highlight = 0;
    int choice = -1;
    int c;

    setlocale(LC_ALL, "");

    initscr();
    waddch(stdscr, 0);

    clear();
    noecho();
    cbreak();
    curs_set(0);

    mvwprintw(stdscr, 1, 1, "=================================================");
    mvwprintw(stdscr, 2, 1, title.c_str());
    mvwprintw(stdscr, 3, 1, "=================================================");
    refresh();

    const int MENU_WIDTH = 55;
    int menu_height = options.size() + 4;

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    int start_y = (max_y - menu_height) / 2;
    int start_x = (max_x - MENU_WIDTH) / 2;

    WINDOW *menu_win = newwin(menu_height, MENU_WIDTH, start_y, start_x);
    keypad(menu_win, TRUE);

    do {
        draw_menu(menu_win, options, highlight);
        c = wgetch(menu_win);

        switch(c) {
            case KEY_UP:
                highlight = (highlight == 0) ? options.size() - 1 : highlight - 1;
                break;
            case KEY_DOWN:
                highlight = (highlight == options.size() - 1) ? 0 : highlight + 1;
                break;
            case 10:
                choice = highlight + 1;
                break;
            default:
                break;
        }
    } while (choice == -1);

    delwin(menu_win);
    endwin();

    return choice;
}

// -------------------------------------------------------------
// LOGICA DE NEGOCIO (ejecutarMenu) 
// -------------------------------------------------------------

void ejecutarMenu() {
    Refugio sistemaRefugio;
    int opcion = 0;

    const vector<string> menuPrincipal = {
            "Animales",
            "Cuidadores",
            "Controles Veterinarios",
            "Adopciones",
            "Reportes y Filtrado",
            "Estadisticas (Dashboard)",
            "Salir"
    };

    const vector<string> menuAnimales = {
            "Listar todos (Consulta)",
            "Alta (Nuevo registro)",
            "Busqueda/Consulta por ID",
            "Modificacion/Funciones especiales",
            "Baja/Eliminacion fisica",
            "Ordenar por Edad",
            "Volver al menu principal"
    };

    const vector<string> menuCuidadores = {
            "Listar todos",
            "Alta de Cuidador",
            "Busqueda por ID",
            "Asignar Animal a Cuidador",
            "Baja de Cuidador",
            "Volver al menu principal"
    };

    // SUBMENÚS DINÁMICOS
    const vector<string> menuControles = {
            "Listar todos",
            "Alta (Registrar nuevo control)",
            "Volver al menu principal"
    };

    const vector<string> menuAdopciones = {
            "Listar todos",
            "Avanzar/Retroceder Estado",
            "Volver al menu principal"
    };

    const vector<string> menuReportes = {
            "Animales por Cuidador",
            "Adopciones por Estado (Aprobadas)",
            "Historial Veterinario por Animal",
            "Volver al menu principal"
    };


    do {
        opcion = run_menu(menuPrincipal, "SISTEMA DE GESTION DE REFUGIO DE ANIMALES");

        if (opcion != 7) {
            show_message_box("Seleccionaste: " + menuPrincipal[opcion - 1] + ".");
        }

        switch (opcion) {
            case 1: { // GESTIÓN DE ANIMALES
                int op;
                op = run_menu(menuAnimales, "--- GESTIÓN DE ANIMALES ---");
                show_message_box("Seleccionaste: " + menuAnimales[op - 1] + ".");

                if (op == 1) { // Listar todos
                    sistemaRefugio.listarGenerico(sistemaRefugio.repoAnimales, "ANIMALES");
                    pausarYLimpiar();
                }
                else if (op == 2) { // Alta
                    string nombre, raza; int edad, id;
                    cout << "ID (unico): ";
                    if (!(cin >> id)) {
                        cout << "ID invalido." << endl;
                        cin.clear(); // Limpia el error de cin
                        limpiarBuffer(); // Limpia el buffer
                        pausarYLimpiar();
                        break;
                    }
                    limpiarBuffer();

                    cout << "Nombre: "; getline(cin, nombre);

                    cout << "Edad: ";
                    if (!(cin >> edad)) {
                        cout << "Edad invalida." << endl;
                        cin.clear(); // Limpia el error de cin
                        limpiarBuffer(); // Limpia el buffer
                        pausarYLimpiar();
                        break;
                    }
                    limpiarBuffer();

                    cout << "Especie (Perro/Gato): "; getline(cin, raza);

                    if (raza == "Perro") sistemaRefugio.altaAnimal(new Perro(id, nombre, edad, ""));
                    else if (raza == "Gato") sistemaRefugio.altaAnimal(new Gato(id, nombre, edad));
                    else cout << "Especie no soportada para Alta." << endl;

                    pausarYLimpiar();
                }
                else if (op == 3) { // Busqueda/Consulta por ID
                    int id;
                    cout << "ID del Animal a buscar: ";
                    if (!(cin >> id)) {
                        cin.clear(); // Limpia el error de cin
                        limpiarBuffer(); // Limpia el buffer
                        pausarYLimpiar();
                        break;
                    }
                    limpiarBuffer();
                    sistemaRefugio.buscarYMostrarAnimal(id);
                    pausarYLimpiar();
                }
                else if (op == 4) { // Modificacion/Funciones especiales
                    int id;
                    cout << "ID del Animal a modificar: ";
                    if (!(cin >> id)) {
                        cin.clear(); // Limpia el error de cin
                        limpiarBuffer(); // Limpia el buffer 
                        pausarYLimpiar();
                        break;
                    }
                    limpiarBuffer();
                    sistemaRefugio.modificarAnimal(id);
                    pausarYLimpiar();
                }
                else if (op == 5) { // Baja/Eliminacion fisica
                    int id;
                    cout << "ID del Animal a eliminar fisicamente: ";
                    if (!(cin >> id)) {
                        cin.clear(); // Limpia el error de cin
                        limpiarBuffer(); // Limpia el buffer 
                        pausarYLimpiar();
                        break;
                    }
                    limpiarBuffer();
                    sistemaRefugio.bajaAnimal(id);
                    pausarYLimpiar();
                }
                else if (op == 6) { // Ordenar por Edad
                    sistemaRefugio.ordenarAnimalesPorEdad();
                    sistemaRefugio.listarGenerico(sistemaRefugio.repoAnimales, "ANIMALES ORDENADOS POR EDAD");
                    pausarYLimpiar();
                }
                break;
            }
            case 2: { // GESTIÓN DE CUIDADORES
                int op;
                op = run_menu(menuCuidadores, "--- GESTIÓN DE CUIDADORES ---");
                show_message_box("Seleccionaste: " + menuCuidadores[op - 1] + ".");

                if (op == 1) { // Listar todos
                    sistemaRefugio.listarGenerico(sistemaRefugio.repoCuidadores, "CUIDADORES");
                    pausarYLimpiar();
                }
                else if (op == 2) { // Alta de Cuidador
                    string nombre; int id, carga;
                    cout << "ID (unico): ";
                    if (!(cin >> id)) {
                        cout << "ID invalido." << endl;
                        cin.clear();
                        limpiarBuffer(); // Limpia el buffer 
                        pausarYLimpiar();
                        break;
                    }
                    limpiarBuffer();

                    cout << "Nombre: "; getline(cin, nombre);

                    cout << "Carga Maxima (ej: 5): ";
                    if (!(cin >> carga)) {
                        cout << "Carga invalida." << endl;
                        cin.clear();
                        limpiarBuffer(); // Limpia el buffer
                        pausarYLimpiar();
                        break;
                    }
                    limpiarBuffer();

                    sistemaRefugio.altaCuidador(new Cuidador(id, nombre, carga));
                    pausarYLimpiar();
                }
                else if (op == 3) { // Busqueda por ID
                    int id;
                    cout << "ID del Cuidador a buscar: ";
                    if (!(cin >> id)) {
                        cin.clear();
                        limpiarBuffer(); // Limpia el buffer 
                        pausarYLimpiar();
                        break;
                    }
                    limpiarBuffer();
                    sistemaRefugio.buscarYMostrarCuidador(id);
                    pausarYLimpiar();
                }
                else if (op == 4) { // Asignar Animal a Cuidador
                    int idC, idA;
                    cout << "--- ASIGNAR ANIMAL A CUIDADOR ---" << endl;
                    cout << "ID del Cuidador: ";
                    if (!(cin >> idC)) {
                        cin.clear();
                        limpiarBuffer(); // Limpia el buffer
                        pausarYLimpiar();
                        break;
                    }
                    limpiarBuffer();

                    cout << "ID del Animal a asignar: ";
                    if (!(cin >> idA)) {
                        cin.clear();
                        limpiarBuffer(); // Limpia el buffer 
                        pausarYLimpiar();
                        break;
                    }
                    limpiarBuffer();

                    sistemaRefugio.asignarAnimalACuidador(idC, idA);
                    pausarYLimpiar();
                }
                else if (op == 5) { // Baja de Cuidador
                    int id;
                    cout << "ID del Cuidador a eliminar: ";
                    if (!(cin >> id)) {
                        cin.clear();
                        limpiarBuffer(); // Limpia el buffer 
                        pausarYLimpiar();
                        break;
                    }
                    limpiarBuffer();
                    sistemaRefugio.bajaCuidador(id);
                    pausarYLimpiar();
                }
                break;
            }
            case 3: { // CONTROLES VETERINARIOS
                int op;
                op = run_menu(menuControles, "--- GESTIÓN DE CONTROLES VETERINARIOS ---");
                show_message_box("Seleccionaste: " + menuControles[op - 1] + ".");

                if (op == 1) { // Listar todos
                    sistemaRefugio.listarGenerico(sistemaRefugio.repoControles, "CONTROLES VETERINARIOS");
                    pausarYLimpiar();
                }
                else if (op == 2) { // Alta (Registrar nuevo control)
                    int idA, idC; string fecha; char tipo;
                    cout << "--- REGISTRAR CONTROL ---" << endl;
                    cout << "ID del Control (unico): ";
                    if (!(cin >> idC)) {
                        cin.clear();
                        limpiarBuffer();
                        pausarYLimpiar();
                        break;
                    }
                    limpiarBuffer();

                    cout << "ID del Animal: ";
                    if (!(cin >> idA)) {
                        cin.clear();
                        limpiarBuffer();
                        pausarYLimpiar();
                        break;
                    }
                    limpiarBuffer();

                    cout << "Fecha (DD/MM/AAAA): "; getline(cin, fecha);

                    cout << "Tipo (V: Vacunacion, D: Desparasitacion): "; cin >> tipo; limpiarBuffer();

                    Animal* a = sistemaRefugio.repoAnimales.buscarPorID(idA);
                    if (!a) { cout << "Animal no encontrado." << endl; }
                    else if (tolower(tipo) == 'v') sistemaRefugio.altaControl(new Vacunacion(idC, fecha, a));
                    else if (tolower(tipo) == 'd') sistemaRefugio.altaControl(new Desparasitacion(idC, fecha, a));
                    else cout << "Tipo de control invalido." << endl;

                    pausarYLimpiar();
                }
                break;
            }
            case 4: { // ADOPCIONES
                int op;
                op = run_menu(menuAdopciones, "--- GESTIÓN DE ADOPCIONES ---");
                show_message_box("Seleccionaste: " + menuAdopciones[op - 1] + ".");

                if (op == 1) { // Listar todos
                    sistemaRefugio.listarGenerico(sistemaRefugio.repoAdopciones, "ADOPCIONES");
                    pausarYLimpiar();
                }
                else if (op == 2) { // Avanzar/Retroceder Estado
                    int idAdop; char accion;
                    cout << "--- SEGUIMIENTO DE TRAMITE ---" << endl;
                    cout << "ID Adopcion: ";
                    if (!(cin >> idAdop)) {
                        cin.clear();
                        limpiarBuffer();
                        pausarYLimpiar();
                        break;
                    }
                    limpiarBuffer();

                    cout << "Avanzar (A) o Retroceder (R) estado?: "; cin >> accion; limpiarBuffer();
                    if (tolower(accion) == 'a') sistemaRefugio.avanzarEstadoAdopcion(idAdop);
                    else if (tolower(accion) == 'r') sistemaRefugio.retrocederEstadoAdopcion(idAdop);
                    else cout << "Accion no reconocida." << endl;

                    pausarYLimpiar();
                }
                break;
            }
            case 5: { // REPORTES Y FILTRADO
                int op;
                op = run_menu(menuReportes, "--- REPORTES Y FILTRADO ---");
                show_message_box("Seleccionaste: " + menuReportes[op - 1] + ".");

                if (op == 1) { // Animales por Cuidador
                    sistemaRefugio.listarReporteCuidadores();
                    pausarYLimpiar();
                }
                else if (op == 2) { // Adopciones por Estado (Aprobadas)
                    sistemaRefugio.listarAdopcionesPorEstado(EstadoAdopcion::APROBADA);
                    pausarYLimpiar();
                }
                else if (op == 3) { // Historial Veterinario por Animal
                    int idA;
                    cout << "ID del animal para historial: ";
                    if (!(cin >> idA)) {
                        cin.clear();
                        limpiarBuffer();
                        pausarYLimpiar();
                        break;
                    }
                    limpiarBuffer();
                    sistemaRefugio.reporteHistorialVeterinario(idA);
                    pausarYLimpiar();
                }
                break;
            }
            case 6: { // ESTADÍSTICAS
                vector<string> menuEstadisticas = {"Mostrar Estadisticas", "Volver al menu principal"};
                int op = run_menu(menuEstadisticas, "--- ESTADÍSTICAS ---");
                show_message_box("Seleccionaste: " + menuEstadisticas[op - 1] + ".");

                if (op == 1) {
                    sistemaRefugio.mostrarEstadisticas();
                    pausarYLimpiar();
                }
                break;
            }
            case 7:
                cout << "Saliendo del sistema. ¡Adios!" << endl;
                break;
            default:
                break;
        }

    } while (opcion != 7);
}

int main() {
    ejecutarMenu();
    return 0;
}*/
#include <iostream>
#include <limits>
#include <string>
#include <cctype>
#include <vector>
#include <algorithm>
#include <locale.h>
#include <cstdlib>
#include <ncurses.h>

// INCLUDES 
#include "Base/Animal.h"
#include "Utilidad/Enums.h"
#include "Core/Refugio.h"
#include "Dominio/Perro.h"
#include "Dominio/Gato.h"
#include "Dominio/Cuidador.h"
#include "Dominio/Adopcion.h"
#include "Dominio/Vacunacion.h"
#include "Dominio/Desparasitacion.h"

using namespace std;

// -------------------------------------------------------------
// FUNCIONES DE UTILIDAD
// -------------------------------------------------------------

void limpiarBuffer() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void pausarYLimpiar() {
    cout << "\nPresione ENTER para continuar...";
    if (cin.fail()) {
        cin.clear();
    }
    limpiarBuffer();
    cin.get();
    
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// -------------------------------------------------------------
// FUNCIONES MODULARES NCURSES
// -------------------------------------------------------------

void draw_menu(WINDOW *menu_win, const vector<string>& options, int highlight) {
    int max_y, max_x, i;
    getmaxyx(menu_win, max_y, max_x);
    int y_start = 2;

    box(menu_win, 0, 0);

    for (i = 0; i < options.size(); ++i) {
        int option_length = options[i].length() + 4;
        int x_start = (max_x - option_length) / 2;

        if (highlight == i) {
            wattron(menu_win, A_REVERSE);
        }

        mvwprintw(menu_win, y_start + i, x_start, "%d. %s", i + 1, options[i].c_str());
        wattroff(menu_win, A_REVERSE);
    }
    wrefresh(menu_win);
}

void show_message_box(const string& message) {
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    // Calcular tamaño dinámico del mensaje
    vector<string> lineas;
    size_t pos = 0;
    int max_width = 0;
    while (pos < message.length()) {
        string linea = message.substr(pos, min((size_t)50, message.length() - pos));
        lineas.push_back(linea);
        max_width = max(max_width, (int)linea.length());
        pos += 50;
    }

    int h = lineas.size() + 4;
    int w = max_width + 6;
    int start_y = (max_y - h) / 2;
    int start_x = (max_x - w) / 2;

    WINDOW *message_win = newwin(h, w, start_y, start_x);
    box(message_win, 0, 0);

    for (int i = 0; i < lineas.size(); i++) {
        mvwprintw(message_win, 1 + i, 2, lineas[i].c_str());
    }
    
    mvwprintw(message_win, h - 2, 2, "Presione ENTER para continuar...");
    wrefresh(message_win);

    int ch;
    while ((ch = wgetch(message_win)) != 10 && ch != '\n');

    delwin(message_win);
    endwin();

#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

int run_menu(const vector<string>& options, const string& title) {
    int highlight = 0;
    int choice = -1;
    int c;

    setlocale(LC_ALL, "");
    initscr();
    clear();
    noecho();
    cbreak();
    curs_set(0);

    // Título
    mvprintw(1, 1, "=================================================");
    mvprintw(2, 1, title.c_str());
    mvprintw(3, 1, "=================================================");
    refresh();

    const int MENU_WIDTH = 55;
    int menu_height = options.size() + 4;

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    int start_y = (max_y - menu_height) / 2;
    int start_x = (max_x - MENU_WIDTH) / 2;

    WINDOW *menu_win = newwin(menu_height, MENU_WIDTH, start_y, start_x);
    keypad(menu_win, TRUE);

    do {
        draw_menu(menu_win, options, highlight);
        c = wgetch(menu_win);

        switch(c) {
            case KEY_UP:
                highlight = (highlight == 0) ? options.size() - 1 : highlight - 1;
                break;
            case KEY_DOWN:
                highlight = (highlight == options.size() - 1) ? 0 : highlight + 1;
                break;
            case 10:
                choice = highlight + 1;
                break;
            default:
                break;
        }
    } while (choice == -1);

    delwin(menu_win);
    endwin();

    return choice;
}

// -------------------------------------------------------------
// FUNCIONES AUXILIARES PARA VALIDACIÓN
// -------------------------------------------------------------

bool idExisteEnAnimales(Refugio& sistema, int id) {
    return sistema.repoAnimales.buscarPorID(id) != nullptr;
}

bool idExisteEnCuidadores(Refugio& sistema, int id) {
    return sistema.repoCuidadores.buscarPorID(id) != nullptr;
}

bool idExisteEnControles(Refugio& sistema, int id) {
    return sistema.repoControles.buscarPorID(id) != nullptr;
}

bool idExisteEnAdopciones(Refugio& sistema, int id) {
    return sistema.repoAdopciones.buscarPorID(id) != nullptr;
}

// -------------------------------------------------------------
// LOGICA DE NEGOCIO (ejecutarMenu) 
// -------------------------------------------------------------

void ejecutarMenu() {
    Refugio sistemaRefugio;
    int opcion = 0;

    const vector<string> menuPrincipal = {
        "Animales",
        "Cuidadores",
        "Controles Veterinarios",
        "Adopciones",
        "Reportes y Filtrado",
        "Estadisticas (Dashboard)",
        "Salir"
    };

    const vector<string> menuAnimales = {
        "Listar todos (Consulta)",
        "Alta (Nuevo registro)",
        "Busqueda/Consulta por ID",
        "Modificacion/Funciones especiales",
        "Baja/Eliminacion fisica",
        "Ordenar por Edad",
        "Volver al menu principal"
    };

    const vector<string> menuCuidadores = {
        "Listar todos",
        "Alta de Cuidador",
        "Busqueda por ID",
        "Asignar Animal a Cuidador",
        "Baja de Cuidador",
        "Volver al menu principal"
    };

    const vector<string> menuControles = {
        "Listar todos",
        "Alta (Registrar nuevo control)",
        "Volver al menu principal"
    };

    // MENÚ DE ADOPCIONES MEJORADO
    const vector<string> menuAdopciones = {
        "Listar todos",
        "Crear nueva adopción",
        "Avanzar Estado",
        "Retroceder Estado",
        "Volver al menu principal"
    };

    const vector<string> menuReportes = {
        "Animales por Cuidador",
        "Adopciones por Estado (Aprobadas)",
        "Historial Veterinario por Animal",
        "Volver al menu principal"
    };

    do {
        opcion = run_menu(menuPrincipal, "SISTEMA DE GESTION DE REFUGIO DE ANIMALES");

        if (opcion != 7) {
            show_message_box("Seleccionaste: " + menuPrincipal[opcion - 1] + ".");
        }

        switch (opcion) {
            case 1: { // GESTIÓN DE ANIMALES
                int op;
                do {
                    op = run_menu(menuAnimales, "--- GESTIÓN DE ANIMALES ---");
                    show_message_box("Seleccionaste: " + menuAnimales[op - 1] + ".");

                    if (op == 1) { // Listar todos
                        sistemaRefugio.listarGenerico(sistemaRefugio.repoAnimales, "ANIMALES");
                        pausarYLimpiar();
                    }
                    else if (op == 2) { // Alta
                        string nombre, raza; int edad, id;
                        cout << "ID (unico): ";
                        if (!(cin >> id)) {
                            cout << "ID invalido." << endl;
                            cin.clear();
                            limpiarBuffer();
                            pausarYLimpiar();
                            break;
                        }
                        
                        if (idExisteEnAnimales(sistemaRefugio, id)) {
                            cout << "ERROR: El ID " << id << " ya existe." << endl;
                            limpiarBuffer();
                            pausarYLimpiar();
                            break;
                        }
                        
                        limpiarBuffer();
                        cout << "Nombre: "; getline(cin, nombre);

                        cout << "Edad: ";
                        if (!(cin >> edad)) {
                            cout << "Edad invalida." << endl;
                            cin.clear();
                            limpiarBuffer();
                            pausarYLimpiar();
                            break;
                        }
                        limpiarBuffer();

                        cout << "Especie (Perro/Gato): "; getline(cin, raza);

                        if (raza == "Perro" || raza == "perro") {
                            string razaPerro;
                            cout << "Raza del perro: ";
                            getline(cin, razaPerro);
                            sistemaRefugio.altaAnimal(new Perro(id, nombre, edad, razaPerro));
                            cout << "Perro registrado exitosamente!" << endl;
                        } else if (raza == "Gato" || raza == "gato") {
                            sistemaRefugio.altaAnimal(new Gato(id, nombre, edad));
                            cout << "Gato registrado exitosamente!" << endl;
                        } else {
                            cout << "Especie no soportada para Alta." << endl;
                        }
                        pausarYLimpiar();
                    }
                    else if (op == 3) { // Busqueda/Consulta por ID
                        int id;
                        cout << "ID del Animal a buscar: ";
                        if (!(cin >> id)) {
                            cin.clear();
                            limpiarBuffer();
                            pausarYLimpiar();
                            break;
                        }
                        limpiarBuffer();
                        sistemaRefugio.buscarYMostrarAnimal(id);
                        pausarYLimpiar();
                    }
                    else if (op == 4) { // Modificacion/Funciones especiales
                        int id;
                        cout << "ID del Animal a modificar: ";
                        if (!(cin >> id)) {
                            cin.clear();
                            limpiarBuffer();
                            pausarYLimpiar();
                            break;
                        }
                        limpiarBuffer();
                        sistemaRefugio.modificarAnimal(id);
                        pausarYLimpiar();
                    }
                    else if (op == 5) { // Baja/Eliminacion fisica
                        int id;
                        cout << "ID del Animal a eliminar fisicamente: ";
                        if (!(cin >> id)) {
                            cin.clear();
                            limpiarBuffer();
                            pausarYLimpiar();
                            break;
                        }
                        limpiarBuffer();
                        sistemaRefugio.bajaAnimal(id);
                        pausarYLimpiar();
                    }
                    else if (op == 6) { // Ordenar por Edad
                        sistemaRefugio.ordenarAnimalesPorEdad();
                        sistemaRefugio.listarGenerico(sistemaRefugio.repoAnimales, "ANIMALES ORDENADOS POR EDAD");
                        pausarYLimpiar();
                    }
                } while (op != 7);
                break;
            }
            
            case 2: { // GESTIÓN DE CUIDADORES
                int op;
                do {
                    op = run_menu(menuCuidadores, "--- GESTIÓN DE CUIDADORES ---");
                    show_message_box("Seleccionaste: " + menuCuidadores[op - 1] + ".");

                    if (op == 1) { // Listar todos
                        sistemaRefugio.listarGenerico(sistemaRefugio.repoCuidadores, "CUIDADORES");
                        pausarYLimpiar();
                    }
                    else if (op == 2) { // Alta de Cuidador
                        string nombre; int id, carga;
                        cout << "ID (unico): ";
                        if (!(cin >> id)) {
                            cout << "ID invalido." << endl;
                            cin.clear();
                            limpiarBuffer();
                            pausarYLimpiar();
                            break;
                        }
                        
                        if (idExisteEnCuidadores(sistemaRefugio, id)) {
                            cout << "ERROR: El ID " << id << " ya existe." << endl;
                            limpiarBuffer();
                            pausarYLimpiar();
                            break;
                        }
                        
                        limpiarBuffer();
                        cout << "Nombre: "; getline(cin, nombre);

                        cout << "Carga Maxima (ej: 5): ";
                        if (!(cin >> carga)) {
                            cout << "Carga invalida." << endl;
                            cin.clear();
                            limpiarBuffer();
                            pausarYLimpiar();
                            break;
                        }
                        limpiarBuffer();

                        sistemaRefugio.altaCuidador(new Cuidador(id, nombre, carga));
                        pausarYLimpiar();
                    }
                    else if (op == 3) { // Busqueda por ID
                        int id;
                        cout << "ID del Cuidador a buscar: ";
                        if (!(cin >> id)) {
                            cin.clear();
                            limpiarBuffer();
                            pausarYLimpiar();
                            break;
                        }
                        limpiarBuffer();
                        sistemaRefugio.buscarYMostrarCuidador(id);
                        pausarYLimpiar();
                    }
                    else if (op == 4) { // Asignar Animal a Cuidador
                        int idC, idA;
                        cout << "--- ASIGNAR ANIMAL A CUIDADOR ---" << endl;
                        cout << "ID del Cuidador: ";
                        if (!(cin >> idC)) {
                            cin.clear();
                            limpiarBuffer();
                            pausarYLimpiar();
                            break;
                        }
                        limpiarBuffer();

                        cout << "ID del Animal a asignar: ";
                        if (!(cin >> idA)) {
                            cin.clear();
                            limpiarBuffer();
                            pausarYLimpiar();
                            break;
                        }
                        limpiarBuffer();

                        sistemaRefugio.asignarAnimalACuidador(idC, idA);
                        pausarYLimpiar();
                    }
                    else if (op == 5) { // Baja de Cuidador
                        int id;
                        cout << "ID del Cuidador a eliminar: ";
                        if (!(cin >> id)) {
                            cin.clear();
                            limpiarBuffer();
                            pausarYLimpiar();
                            break;
                        }
                        limpiarBuffer();
                        sistemaRefugio.bajaCuidador(id);
                        pausarYLimpiar();
                    }
                } while (op != 6);
                break;
            }
            
            case 3: { // CONTROLES VETERINARIOS
                int op;
                do {
                    op = run_menu(menuControles, "--- GESTIÓN DE CONTROLES VETERINARIOS ---");
                    show_message_box("Seleccionaste: " + menuControles[op - 1] + ".");

                    if (op == 1) { // Listar todos
                        sistemaRefugio.listarGenerico(sistemaRefugio.repoControles, "CONTROLES VETERINARIOS");
                        pausarYLimpiar();
                    }
                    else if (op == 2) { // Alta (Registrar nuevo control)
                        int idA, idC; string fecha; char tipo;
                        cout << "--- REGISTRAR CONTROL ---" << endl;
                        cout << "ID del Control (unico): ";
                        if (!(cin >> idC)) {
                            cin.clear();
                            limpiarBuffer();
                            pausarYLimpiar();
                            break;
                        }
                        
                        if (idExisteEnControles(sistemaRefugio, idC)) {
                            cout << "ERROR: El ID " << idC << " ya existe." << endl;
                            limpiarBuffer();
                            pausarYLimpiar();
                            break;
                        }
                        
                        limpiarBuffer();
                        cout << "ID del Animal: ";
                        if (!(cin >> idA)) {
                            cin.clear();
                            limpiarBuffer();
                            pausarYLimpiar();
                            break;
                        }
                        limpiarBuffer();

                        cout << "Fecha (DD/MM/AAAA): "; getline(cin, fecha);

                        cout << "Tipo (V: Vacunacion, D: Desparasitacion): "; cin >> tipo; limpiarBuffer();

                        Animal* a = sistemaRefugio.repoAnimales.buscarPorID(idA);
                        if (!a) { 
                            cout << "Animal no encontrado." << endl; 
                        } else if (tolower(tipo) == 'v') {
                            sistemaRefugio.altaControl(new Vacunacion(idC, fecha, a));
                            cout << "Vacunacion registrada exitosamente!" << endl;
                        } else if (tolower(tipo) == 'd') {
                            sistemaRefugio.altaControl(new Desparasitacion(idC, fecha, a));
                            cout << "Desparasitacion registrada exitosamente!" << endl;
                        } else {
                            cout << "Tipo de control invalido." << endl;
                        }
                        pausarYLimpiar();
                    }
                } while (op != 3);
                break;
            }
            
            case 4: { // ADOPCIONES - SISTEMA COMPLETO
                int op;
                do {
                    op = run_menu(menuAdopciones, "--- GESTIÓN DE ADOPCIONES ---");
                    show_message_box("Seleccionaste: " + menuAdopciones[op - 1] + ".");

                    if (op == 1) { // Listar todos
                        sistemaRefugio.listarGenerico(sistemaRefugio.repoAdopciones, "ADOPCIONES");
                        pausarYLimpiar();
                    }
                    else if (op == 2) { // Crear nueva adopción
                        int idAdopcion, idAnimal;
                        double costo;
                        
                        cout << "--- CREAR NUEVA ADOPCIÓN ---" << endl;
                        
                        // ID Adopción
                        cout << "ID de la adopción (único): ";
                        if (!(cin >> idAdopcion)) {
                            cin.clear();
                            limpiarBuffer();
                            cout << "ID inválido." << endl;
                            pausarYLimpiar();
                            break;
                        }
                        
                        if (idExisteEnAdopciones(sistemaRefugio, idAdopcion)) {
                            cout << "ERROR: El ID de adopción " << idAdopcion << " ya existe." << endl;
                            limpiarBuffer();
                            pausarYLimpiar();
                            break;
                        }
                        
                        limpiarBuffer();
                        
                        // Mostrar animales disponibles para adopción
                        cout << "\n--- ANIMALES DISPONIBLES PARA ADOPCIÓN ---" << endl;
                        bool hayAnimalesDisponibles = false;
                        for (const auto& animal : sistemaRefugio.repoAnimales) {
                            if (!animal->estaAdoptado()) {
                                cout << "ID: " << animal->getId() 
                                     << " | Nombre: " << animal->getNombre() 
                                     << " | Especie: " << animal->getEspecie() 
                                     << " | Edad: " << animal->getEdad() << " años" << endl;
                                hayAnimalesDisponibles = true;
                            }
                        }
                        
                        if (!hayAnimalesDisponibles) {
                            cout << "No hay animales disponibles para adopción." << endl;
                            pausarYLimpiar();
                            break;
                        }
                        
                        cout << "\n----------------------------------------" << endl;
                        
                        // ID Animal
                        cout << "ID del Animal a adoptar: ";
                        if (!(cin >> idAnimal)) {
                            cin.clear();
                            limpiarBuffer();
                            cout << "ID inválido." << endl;
                            pausarYLimpiar();
                            break;
                        }
                        limpiarBuffer();
                        
                        // Verificar que el animal existe y no está adoptado
                        Animal* animalSeleccionado = sistemaRefugio.repoAnimales.buscarPorID(idAnimal);
                        if (!animalSeleccionado) {
                            cout << "ERROR: No existe un animal con ID " << idAnimal << endl;
                            pausarYLimpiar();
                            break;
                        }
                        
                        if (animalSeleccionado->estaAdoptado()) {
                            cout << "ERROR: El animal " << animalSeleccionado->getNombre() 
                                 << " ya está adoptado." << endl;
                            pausarYLimpiar();
                            break;
                        }
                        
                        // Mostrar información del animal seleccionado
                        cout << "\n--- INFORMACIÓN DEL ANIMAL SELECCIONADO ---" << endl;
                        cout << "Nombre: " << animalSeleccionado->getNombre() << endl;
                        cout << "Especie: " << animalSeleccionado->getEspecie() << endl;
                        cout << "Tipo de animal: " << animalSeleccionado->getEspecie() << endl;
                        cout << "Edad: " << animalSeleccionado->getEdad() << " años" << endl;
                        cout << "ID: " << animalSeleccionado->getId() << endl;
                        
                        // Costo
                        cout << "\nCosto base de la adopción: $";
                        if (!(cin >> costo) || costo < 0) {
                            cin.clear();
                            limpiarBuffer();
                            cout << "Costo inválido." << endl;
                            pausarYLimpiar();
                            break;
                        }
                        limpiarBuffer();
                        
                        // Confirmación final
                        cout << "\n--- RESUMEN DE LA ADOPCIÓN ---" << endl;
                        cout << "ID Adopción: " << idAdopcion << endl;
                        cout << "Animal: " << animalSeleccionado->getNombre() << " (" << animalSeleccionado->getEspecie() << ")" << endl;
                        cout << "ID Animal: " << idAnimal << endl;
                        cout << "Costo base: $" << costo << endl;
                        cout << "Costo total (con 15% de gestión): $" << (costo * 1.15) << endl;
                        
                        char confirmar;
                        cout << "\n¿Confirmar la adopción? (S/N): ";
                        cin >> confirmar;
                        limpiarBuffer();
                        
                        if (tolower(confirmar) == 's') {
                            try {
                                sistemaRefugio.crearAdopcion(idAdopcion, idAnimal, costo);
                                cout << "Adopción registrada exitosamente!" << endl;
                                cout << "Estado inicial: " << estadoToString(EstadoAdopcion::INICIADA) << endl;
                            } catch (const std::runtime_error& e) {
                                cout << "Error: " << e.what() << endl;
                            }
                        } else {
                            cout << "Adopción cancelada." << endl;
                        }
                        
                        pausarYLimpiar();
                    }
                    else if (op == 3) { // Avanzar Estado
                        int idAdopcion;
                        cout << "--- AVANZAR ESTADO DE ADOPCIÓN ---" << endl;
                        
                        // Mostrar adopciones existentes
                        cout << "\n--- ADOPCIONES REGISTRADAS ---" << endl;
                        if (sistemaRefugio.repoAdopciones.getCantidad() == 0) {
                            cout << "No hay adopciones registradas." << endl;
                            pausarYLimpiar();
                            break;
                        }
                        
                        for (const auto& adopcion : sistemaRefugio.repoAdopciones) {
                            cout << "ID: " << adopcion->getId() 
                                 << " | Animal: " << adopcion->getAnimal()->getNombre()
                                 << " | Estado: " << estadoToString(adopcion->getEstado()) << endl;
                        }
                        
                        cout << "\nID Adopción a avanzar: ";
                        if (!(cin >> idAdopcion)) {
                            cin.clear();
                            limpiarBuffer();
                            cout << "ID inválido." << endl;
                            pausarYLimpiar();
                            break;
                        }
                        limpiarBuffer();
                        
                        sistemaRefugio.avanzarEstadoAdopcion(idAdopcion);
                        pausarYLimpiar();
                    }
                    else if (op == 4) { // Retroceder Estado
                        int idAdopcion;
                        cout << "--- RETROCEDER ESTADO DE ADOPCIÓN ---" << endl;
                        
                        // Mostrar adopciones existentes
                        cout << "\n--- ADOPCIONES REGISTRADAS ---" << endl;
                        if (sistemaRefugio.repoAdopciones.getCantidad() == 0) {
                            cout << "No hay adopciones registradas." << endl;
                            pausarYLimpiar();
                            break;
                        }
                        
                        for (const auto& adopcion : sistemaRefugio.repoAdopciones) {
                            cout << "ID: " << adopcion->getId() 
                                 << " | Animal: " << adopcion->getAnimal()->getNombre()
                                 << " | Estado: " << estadoToString(adopcion->getEstado()) << endl;
                        }
                        
                        cout << "\nID Adopción a retroceder: ";
                        if (!(cin >> idAdopcion)) {
                            cin.clear();
                            limpiarBuffer();
                            cout << "ID inválido." << endl;
                            pausarYLimpiar();
                            break;
                        }
                        limpiarBuffer();
                        
                        sistemaRefugio.retrocederEstadoAdopcion(idAdopcion);
                        pausarYLimpiar();
                    }
                } while (op != 5);
                break;
            }
            
            case 5: { // REPORTES Y FILTRADO
                int op;
                do {
                    op = run_menu(menuReportes, "--- REPORTES Y FILTRADO ---");
                    show_message_box("Seleccionaste: " + menuReportes[op - 1] + ".");

                    if (op == 1) { // Animales por Cuidador
                        sistemaRefugio.listarReporteCuidadores();
                        pausarYLimpiar();
                    }
                    else if (op == 2) { // Adopciones por Estado (Aprobadas)
                        sistemaRefugio.listarAdopcionesPorEstado(EstadoAdopcion::APROBADA);
                        pausarYLimpiar();
                    }
                    else if (op == 3) { // Historial Veterinario por Animal
                        int idA;
                        cout << "ID del animal para historial: ";
                        if (!(cin >> idA)) {
                            cin.clear();
                            limpiarBuffer();
                            pausarYLimpiar();
                            break;
                        }
                        limpiarBuffer();
                        sistemaRefugio.reporteHistorialVeterinario(idA);
                        pausarYLimpiar();
                    }
                } while (op != 4);
                break;
            }
            
            case 6: { // ESTADÍSTICAS
                vector<string> menuEstadisticas = {"Mostrar Estadisticas", "Volver al menu principal"};
                int op = run_menu(menuEstadisticas, "--- ESTADÍSTICAS ---");
                show_message_box("Seleccionaste: " + menuEstadisticas[op - 1] + ".");

                if (op == 1) {
                    sistemaRefugio.mostrarEstadisticas();
                    pausarYLimpiar();
                }
                break;
            }
            
            case 7:
                cout << "Saliendo del sistema. ¡Adios!" << endl;
                break;
            default:
                break;
        }

    } while (opcion != 7);
}

int main() {
    ejecutarMenu();
    return 0;
}
