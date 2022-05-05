/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/main.cc to edit this template
 */

/* 
 * File:   main.cpp
 * Author: Josue Alvarez M
 *
 * Created on 25 de marzo de 2022, 17:46
 */

#include <iostream>
#include "rapidxml/rapidxml_ext.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include <sstream>
#include <fstream>
#include <bits/stdc++.h>

#include <iostream>
#include <fstream>

#include <codecvt>
#include <fcntl.h>
#include <io.h>

using namespace std;
using namespace rapidxml;

xml_document<> raiz; // Arbol DOM

double* recta(double x1, double y1, double x2, double y2){
    double m = (y2 - y1) / (x2 - x1);
    double b = y1 - (m*x1);
    
    double rectaInf[2] = {m, b};
    double* rectaInfPuntero = rectaInf;
    
    return rectaInfPuntero;
}

char* strToChar(string str){
    int tam = str.size();
    char* arr = new char[tam];

    int i;
    for (i = 0; i < tam; i++) {
        arr[i] = str[i];
    }
    
    return arr;
}

class Path{
private:
    xml_node<>* nodoXML;
    
    // puntos de inicio y fin del area
    double areaInicio[2];
    double areaFin[2];
    
    string id = "";
    string opacidad = "";
    
    string color = "#000000";
    int r = 0;
    int g = 0;
    int b = 0;
    
    //coordenada indicada por moveto
    double M_x = -1;
    double M_y = -1;
    double* M = new double(2);
    
    string d;
    vector<vector<double>> vectorD;
    
    // posibles posiciones donde se moverá el path
    vector<double*> posiciones;
    int posTam = 0;
    
public:
    
    void addPosicion(double* pos){
        posiciones.push_back(pos);
        posTam++;
    }
    
    void setNodoXML(xml_node<>* nodoXML) {
        this->nodoXML = nodoXML;
    }

    void setColor(char* color) {
        this->color = color;
        sscanf(color, "#%02x%02x%02x", &r, &g, &b);
    }
    
    void setId(string id) {
        this->id = id;
    }

    void setOpacidad(string opacidad) {
        this->opacidad = opacidad;
    }
    
    void mover(double* M) {
        // Actualiza los puntos del path
        vector<double> instruc; // Vector de datos de la instrucción
        
        int tam = this->vectorD.size();
        for (int i = 0; i < tam; i++) {
           instruc = this->vectorD[i]; // Selecciona la instrucción del path
           
           double dato;
           int cont = 0;
           int tamInstuc = instruc.size();
           // Recorre la instrucción
           for (int j = 0; j < tamInstuc; j++) {
                dato = instruc[j]; // Selecciona el dato almacenado de la instrucción
                
                if(cont == 0){ // El dato es el código de instrucción
                    // Si la instrucción es relativa no actualiza nada
                    if(int('a') <= dato && dato <= int('z') && (dato != int('m'))) 
                        break;
                    cont = 1;
                }
                else{ // El dato es una coordenada x o y
                    if(cont == 1){ // X
                        this->vectorD[i][j] = abs(this->vectorD[i][j]) - abs(this->M_x) + M[0];
                        cont = 2;
                    }
                    else{ // Y
                        this->vectorD[i][j] = abs(this->vectorD[i][j]) - abs(this->M_y) + M[1];
                        cont = 1;
                    }
                }
            }
        }
        
        //Actualiza el moveto
        this->M = M;
        this->M_x = M[0];
        this->M_y = M[1];
    }

    
    string getId() const {
        return id;
    }

    string getOpacidad() const {
        return opacidad;
    }
    
    vector<double*> getPosiciones() const {
        return posiciones;
    }
    
    int getPosTam() const {
        return posTam;
    }

    
    double* getPosMoveto() const {
        this->M[0] = M_x;
        this->M[1] = M_y;
        
        return M;
    }
    
    string getColor() const {
        return color;
    }
    int* getColorRGB() const {
        int colorRGB[3] = {r, g, b};
        int* colorRGBPuntero = colorRGB;
        return colorRGBPuntero;
    }
    

    double* getAreaFin(){
        return areaFin;
    }
    double* getAreaInicio(){
        return areaInicio;
    }

    
    bool isArea(double x, double y){
        return x >= this->areaInicio[0] && y >= this->areaInicio[1] && x <= this->areaFin[0] && y <= this->areaFin[1];
    }
    
    /*
     * Pasa el path a un vector de double
     */
    void pasarAVector(){
        vector<double>* instruc = new vector<double>();
        
        string numStr = ""; // donde se gurdan los char del numero a pasar a double
        
        bool punto = false; // si ya se ha agregado al número un '.'
        
        int pos = 0; // posicion a insertar en aux
        double aux[2]; // guarda el punto encontrado y que se debe procesar
        
        int tam = this->d.size();
        for (int i = 0; i < tam; i++) {
            char c = this->d[i];
            
            // Pasa los char del número al string
            if((c == '.' && !punto) || (c == '-' && numStr == "") || (int('0') <= int(c) && int(c) <= int('9'))){
                numStr += c;
                if(c == '.')
                    punto = true;
            }
            else{
                // si el char es un código
                if((int('A') <= int(c) && int(c) <= int('Z')) || (int('a') <= int(c) && int(c) <= int('z'))){
                    
                    // Guarda la instrucción anterior
                    if(!instruc->empty()){
                        vectorD.push_back(*instruc);
                        instruc = new vector<double>();
                    }
                    
                    // Guarda el código de instrucción actual
                    instruc->push_back(double(c));
                }
                
                if(numStr != ""){
                    // Define el punto en aux
                    aux[pos] = stod(numStr);

                    pos++;
                    
                    if(pos == 2){
                        // Pasa la coordenada en aux a relativo
                        
                        instruc->push_back(aux[0]);
                        instruc->push_back(aux[1]);
                        
                        pos = 0;
                        aux[0] = 0;
                        aux[1] = 0;
                    }
                    
                    numStr = "";
                    punto = false;
                }
            }
            
            if(c == '-' && numStr == "")
                numStr += c;
        }
        
        if(!instruc->empty())
            vectorD.push_back(*instruc);
    }
    
    /*
     * Analiza y define el area según la información del path entrante
     */
    void path(string infoPath){
        if(toupper(infoPath[infoPath.size()-1]) != 'Z')
            infoPath += 'z'; // Agrega 'z' para marcar el final del path
        
        this->d = infoPath;
        
        string numStr = ""; // donde se gurdan los char del numero a pasar a double
        char codigoNuevo = ' '; // codigo que se acaba de encontrar
        char codigo = ' '; // codigo el cual se está recorriendo
        
        bool punto = false; // si ya se ha agregado al número un '.'
        // coordenades del area del path (menor y mayor)
        double xMen = -1; 
        double yMen = -1;
        double xMay = -1; 
        double yMay = -1; 
        
        int pos = 0; // posicion a insertar en aux
        double aux[2]; // guarda el punto encontrado y que se debe procesar
        double temp;
        
        int tam = infoPath.size();
        for (int i = 0; i < tam; i++) {
            char c = infoPath[i];
            
            // Pasa los char del número al string
            if((c == '.' && !punto) || (c == '-' && numStr == "") || (int('0') <= int(c) && int(c) <= int('9'))){
                numStr += c;
                if(c == '.')
                    punto = true;
            }
            else{
                // si el char es un código
                if(int('A') <= int(c) && int(c) <= int('Z') || int('a') <= int(c) && int(c) <= int('z')){
                    codigoNuevo = c;
                    if(codigo == 'M'){
                        // Quitar moveto del d
                        this->d = this->d.substr(i, tam - i);
                    }
                }
                if(numStr != ""){
                    if(codigo == 'M'){
                        // Define la coordenada del moveto
                        if(M_x == -1)
                            M_x = stod(numStr);
                        else if(M_y == -1){
                            M_y = stod(numStr);
                            
                            xMen = M_x;
                            yMen = M_y;
                        }
                    }
                    else{
                        bool cambio = true; // indica si ha ocurrido un combio en alguna coordenada mayor o menor
                        while(cambio && pos == 2){ // continua si ha ocurrido un cambio
                            cambio = false;
                            // Proceso para encontrar el area del path
                            if(xMen == -1 || aux[0] < xMen){
                                temp = xMen;
                                xMen = aux[0];
                                aux[0] = temp;
                                cambio = true;
                            }
                            if(yMen == -1 || aux[1] < yMen){
                                temp = yMen;
                                yMen = aux[1];
                                aux[1] = temp;
                                cambio = true;
                            }
                            if(xMay == -1 || aux[0] > xMay){
                                temp = xMay;
                                xMay = aux[0];
                                aux[0] = temp;
                                cambio = true;
                            }
                            if(yMay == -1 || aux[1] > yMay){
                                temp = yMay;
                                yMay = aux[1];
                                aux[1] = temp;
                                cambio = true;
                            }

                            pos = 0;
                            aux[0] = 0;
                            aux[1] = 0;
                        }
                        // Define el punto en aux
                        aux[pos] = stod(numStr);

                        // Si es una posicion relativa la pasa a absoluta
                        if(int('a') <= int(codigo) && int(codigo) <= int('z')){
                            if(pos == 0)
                                aux[0] += M_x;
                            else
                                aux[1] += M_y;
                        }

                        pos++;

                    }
                    numStr = "";
                    punto = false;
                }
                
                codigo = codigoNuevo;
            }
            
            if(c == '-' && numStr == "")
                numStr += c;
        }
        
        this->M[0] = M_x;
        this->M[1] = M_y;
        
        this->areaInicio[0] = xMen;
        this->areaInicio[1] = yMen;
        this->areaFin[0] = xMay;
        this->areaFin[1] = yMay;
    }
    
    void guardarPath(){
        string* pathGuardar = new string();
        
        // Generar nuevo string del path
        string path = "M" + to_string(this->M_x) + ',' + to_string(this->M_y);
        vector<double> instruc; // Vector de datos de la instrucción
        
        int tam = this->vectorD.size();
        for (int i = 0; i < tam; i++) {
           instruc = this->vectorD[i]; // Selecciona la instrucción del path
           
           double dato;
           int cont = 0;
           int tamInstuc = instruc.size();
           // Recorre la instrucción
           for (int j = 0; j < tamInstuc; j++) {
                dato = instruc[j]; // Selecciona el dato almacenado de la instrucción
                
                if(cont == 0){ // El dato es el código de instrucción
                    cont++;
                    
                    if(path != "" && path[path.size()-1] == ',') // Quita el ultimo char de ser este una coma
                        path = path.substr(0, path.size()-1);
                    
                    path += char(dato);
                }
                else // El dato es una coordenada x o y
                    path += to_string(dato) + ',';
            }
        }
        pathGuardar[0] = path;
        
        
        // Busca el 'd' y reemplaza su valor por el sting generado
        for (xml_attribute<>* a = nodoXML->first_attribute(); a != NULL; a = a->next_attribute()) {
                string atrNombre = (string)a->name();
                
                for_each(atrNombre.begin(), atrNombre.end(), [](char & c){
                    c = ::tolower(c);
                });
                    
                if(atrNombre == "d")
                    a->value(pathGuardar[0].c_str());
        }
    }
    
    void posicionesBorrarCant(int cant){
        vector<double*>::iterator itPosIni = this->posiciones.begin();
        this->posiciones.erase(itPosIni, itPosIni + cant - 1);
    }
    
    void imprimir(){
        cout << this->id << ";" << this->color << ";" << this->opacidad << ";M" << this->M_x << "," << this->M_y << this->d << endl;
    }
    
    void imprimirPosiciones(){
        double* punto;
        vector<double*>::iterator fin = this->posiciones.end();
        for(vector<double*>::iterator it = this->posiciones.begin(); it != fin; ++it)
        {   
            punto = ((double*)*it);
            cout << punto[0] << ", " << punto[1] << "\n";
        }
        cout << endl;
    }
};




class XML{
private:
    string nombre; // Nombre original del archivo
    file<> xmlFile();
    
    double ancho, alto;
    
    vector<Path*> paths = {};
    
    // puntos de inicio y fin del area
    double* areaInicio = NULL;
    double* areaFin = NULL;
    
    
    vector<Path*> seleccionar(double puntos[][2], int tamP, int colores[][3], int tamC){
        /*
         *  Programación Dinámica
         * 
         *  Al creara el objeto XML se carga la información de cada path en objetos path 
         *  y se define un area general de corte. O(n*m)
         *  
         *  1. Recorre los puntos indicados excluyendo los que están fuera del area general. O(n)
         *  2. Recorre los paths excluyendo los que no tengan un color cercano a uno indicado. O(n*m)
         *  3. Recorre los paths de corte validando si su area coincide con u punto de corte,
         *  de ser así lo agrega al vector pathSeleccionados. O(n*m)
         * 
         *  Prepara los paths para la función de ruta: Pasa la información del path a un vector O(n*m)
         */
        
        vector<Path*> pathSeleccionados = {};
        vector<Path*> pathCorte = {};
        vector<double*> puntosCorte = {};
        
        // 1. Recorre los puntos excluyendo los que están fuera del area
        for (int i = 0; i < tamP; i++) {
            double* p = puntos[i];

            // si no está en el area del xml (area de corte)
            if(!isArea(p[0], p[1]))
                continue;

            puntosCorte.push_back(p);
        }
        
        // 2. Recorre los paths excluyendo los que no tengan un color cercano a uno indicado.
        int rango = 15;
        int* c;
        int* pathColor;
        Path* p;
        vector<Path*>::iterator fin = this->paths.end();
        for(vector<Path*>::iterator it = this->paths.begin(); it != fin; ++it) // Recorre los paths
        {   
            p = ((Path*)*it);
            pathColor = p->getColorRGB(); // selecciona el color rgb del path
            
            for (int i = 0; i < tamC; i++) {
                c = colores[i];
                
                if(c[0] - rango <= pathColor[0] && c[1] - rango <= pathColor[1] && c[2] - rango <= pathColor[2]
                   && c[0] + rango >= pathColor[0] && c[1] + rango >= pathColor[1] && c[2] + rango >= pathColor[2]){
                    
                    pathCorte.push_back(p);
                    break;
                }
            }
        }
        
        // 3. Recorre los paths de corte y los puntos de corte validando si coinciden
        double* punto;
        fin = pathCorte.end();
        vector<double*>::iterator finPuntos = puntosCorte.end();
        for(vector<Path*>::iterator it = pathCorte.begin(); it != fin; ++it) // Recorre los paths
        {   
            p = ((Path*)*it);
            
            for(vector<double*>::iterator itPuntos = puntosCorte.begin(); itPuntos != finPuntos; ++itPuntos) // Recorre los puntos
            {   
                punto = ((double*)*itPuntos);
                if(p->isArea(punto[0], punto[1])){ // si el punto está en el area del path
                    pathSeleccionados.push_back(p);
                    break;
                }
            }
        }
        
        // Pasa la información del path a un vector
        fin = pathSeleccionados.end();
        for(vector<Path*>::iterator it = pathSeleccionados.begin(); it != fin; ++it) // Recorre los paths
        {   
            p = ((Path*)*it);
            p->pasarAVector();
        }
        
        return pathSeleccionados;
    }
    
    void ruta(vector<Path*> pathSeleccionados, double angulo){
        /*  
         *  Recorre los paths seleccionados y para cada path calcula la ruta (recta)
         *  de ser necesario. De ser un angulo recto incrementa el 'x' o 'y'.
         *  Luego guarda todos los puntos calculados en el objeto path.
         * 
         *  El incremento base es 1 en 'x' o 'y'
         * 
         */
        double* posAct;
        double xSig;
        double ySig;
        
        // marca si es un angulo negativo
        bool negativo = false;
        if(angulo < 0)
            negativo = true;
        
        angulo = abs(angulo);
        
        // Pasa el angulo a uno equivalente < 360
        double aux;
        if(angulo > 360){
            aux = ((double)(angulo/360)); // porcentaje
            angulo = (aux - ((int)aux)) * 360; // le quita la parte entera y define el angulo equivalente
        }
        
        // Pasa el angulo negativo a uno equivalente positivo
        if(negativo)
            angulo = 360 - angulo;
        
        // define el angulo en radianes
        double radianes = (angulo / 180) * M_PI;
        
        double x;
        double y;
        
        Path* p;
        vector<Path*>::iterator fin = pathSeleccionados.end();
        for(vector<Path*>::iterator it = pathSeleccionados.begin(); it != fin; ++it) // Recorre los paths
        {   
            p = ((Path*)*it);
            posAct = p->getPosMoveto();
            
            // Si no es un ángulo recto
            if(angulo != 0 && angulo != 90 && angulo != 180 && angulo != 270 && angulo != 360){ // si el angulo no es recto
                
                // Proceso para definir la posición final
                double xDif;
                double yDif;
                // Si está en el I o IV Cuadrante
                if((90 > angulo && angulo > 0) || (360 > angulo && angulo > 270)){
                    xDif = ancho - posAct[0];
                    yDif = tan(radianes) * xDif * -1;
                    x = ancho;
                }
                else{
                    xDif = posAct[0];
                    yDif = tan(radianes) * xDif;
                    x = 0;
                }
                y = posAct[1] + yDif;

                // Si el 'Y' sobrepasó la imagen
                if(y < 0 || y > alto){
                    if(y < 0){
                        yDif = posAct[1];
                        y = 0;
                    }
                    else if(y > alto){
                        yDif = alto - posAct[1];
                        y = alto;
                    }
                    xDif = abs(yDif / tan(radianes));

                    // Si está en el I o IV Cuadrante
                    if((90 > angulo && angulo > 0) || (360 > angulo && angulo > 270))
                        x = posAct[0] + xDif;
                    else
                        x = posAct[0] - xDif;
                }
                
                // (x, y) Es la posición final
                // Proceso para hayar los puntos de la recta
                xSig = posAct[0];
                ySig = posAct[1];
                double* rectaInf = recta(xSig, ySig, x, y); // retorna {m, b}
                double m = rectaInf[0];
                double b = rectaInf[1];
                
                xDif = x - xSig;
                yDif = y - ySig;
                // Define los puntos según x
                if(abs(xDif) > abs(yDif)){ 
                    double incremento = xDif / abs(xDif);
                    
                    while((incremento == 1 && xSig < x) || (incremento == -1 && xSig > x)){
                        xSig += incremento;
                        ySig = (m * xSig) + b;
                        
                        double* punto = new double(2);
                        punto[0] = xSig;
                        punto[1] = ySig;
                        p->addPosicion(punto);
                    }
                }
                else{// Define los puntos según y
                    double incremento = yDif / abs(yDif);
                    
                    while((incremento == 1 && ySig < y) || (incremento == -1 && ySig > y)){
                        ySig += incremento;
                        xSig = (ySig - b) / m;
                        
                        double* punto = new double(2);
                        punto[0] = xSig;
                        punto[1] = ySig;
                        p->addPosicion(punto);
                    }
                }
            }
            else{ // Es un ángulo recto
                xSig = posAct[0];
                ySig = posAct[1];
                // mientras la posición no esté fuera de la imagen
                while(0 < xSig && xSig < this->ancho && 
                      0 < ySig && ySig < this->alto){

                    if(angulo == 0 || angulo == 360)
                        xSig++;
                    else if(angulo == 180)
                        xSig--;
                    else if(angulo == 270)
                        ySig++;
                    else if(angulo == 90)
                        ySig--;
                    
                    double* posSig = new double(2);
                    posSig[0] = xSig;
                    posSig[1] = ySig;
                    p->addPosicion(posSig);
                }
            }
            
        }
    }
    
    void guardarFrame(int frameNum){
        string nuevoNombre = this->nombre + to_string(frameNum) + ".svg";
        
        
        stringstream ss;
        ss << *raiz.first_node(); // Pasa el nodo raiz al stringstream (ss)
        string stringXML = ss.str(); // Pasa a string el xml
        
        
        ofstream file(nuevoNombre.c_str());
        file << stringXML; // Guarda el stringXML en el nuevo archivo
        file.close();
    }
    
    void frame(vector<Path*> pathSeleccionados, int frames){
        /*  Voraz
         * 
         *  Las fases serían los frames a generar
         *  
         *  Óptimo: Posición (punto) a mover el path
         *  Criterio: Punto que concuerde con la cantidad de puntos
         *  dividido entre los frames
         */
        
        // Descarta los paths que apenas se moverían
        Path* p;
        vector<Path*>::iterator fin = pathSeleccionados.end();
        int pos = 1;
        for(vector<Path*>::iterator it = pathSeleccionados.begin(); it != fin; ++it) // Recorre los paths
        {   
            p = ((Path*)*it); // Path
            
            // Borra el path Si la cantidad de puntos es menor al 50% de los frames
            if(p->getPosTam() < frames * 0.5){
                it = pathSeleccionados.erase(it);
                --it;
                pos--;
            }
            
            pos++;
        }
        
        int tamPath = pathSeleccionados.size();
        fin = pathSeleccionados.end();
        for (int etapa = 1; etapa <= frames; etapa++) { // Recorre los paths para cada frame
            for(vector<Path*>::iterator it = pathSeleccionados.begin(); it != fin; ++it)
            {   
                p = ((Path*)*it); // Path
                
                // Criterio
                // incremento aproximado para la posición siguiente
                int pos = p->getPosTam() / frames; // Posición del punto del path a seleccionar para el frame (p->getPosTam() cantidad de puntos)
                
                // Borra los puntos que estén antes del que se va a utilizar
                if(pos-1 > 0)
                    p->posicionesBorrarCant(pos);
                
                double* punto = ((double*)*(p->getPosiciones().begin()));
                double* puntoMover = new double[2];
                puntoMover[0] = punto[0];
                puntoMover[1] = punto[1];
                
                p->mover(puntoMover);
                p->guardarPath();
            }
            // Se ha actualizado las posiciones de los paths
            // Guarda el xml como nuevo frame
            guardarFrame(etapa);
        }
    }
    
    void imprimir(xml_node<>* nodo){
        // Imprime etiquetas y sus valores
        cout << "Etiqueta: " << nodo->name() << endl;
        for (xml_attribute<>* a = nodo->first_attribute(); a != NULL; a = a->next_attribute()) {
            cout << "\tAtributo: " << a->name();
            cout << "\tValor: " << a->value();
        }
        
        // Recorre el primer hijo
        xml_node<>* nodoNuevo = nodo->first_node(); 
        if(nodoNuevo != NULL)
            imprimir(nodoNuevo);
        // Si no tiene primer hijo recorre a su siguiente hermano
        nodoNuevo = nodo->next_sibling(); 
        if(nodoNuevo != NULL)
            imprimir(nodoNuevo);
    }
    
    /*
     * Lee el xml para definir el ancho y altura
     */
    bool setTam(xml_node<>* nodo){
        if((string)nodo->name() == "svg"){
            for (xml_attribute<>* a = nodo->first_attribute(); a != NULL; a = a->next_attribute()) {
                if((string)a->name() == "viewBox"){
                    string valores[4] = {};
                    //split
                    stringstream ss(a->value());
                    string v;
                    int i = 0;
                    while (ss >> v) {
                        valores[i] = v;
                        i++;
                    }

                    this->ancho = strtod(valores[2].c_str(), NULL);
                    this->alto = strtod(valores[3].c_str(), NULL);
                    
                    return true;
                }
            }
        }
        
        // Recorre el primer hijo
        xml_node<>* nodoNuevo = nodo->first_node(); 
        if(nodoNuevo != NULL && setTam(nodoNuevo))
            return true;
        // Si no tiene primer hijo recorre a su siguiente hermano
        nodoNuevo = nodo->next_sibling(); 
        if(nodoNuevo != NULL && setTam(nodoNuevo))
            return true;
        
        return false;
    }
    
    void agregarPath(Path* p){
        this->paths.push_back(p);
        
        double* areaIniPath = p->getAreaInicio();
        double* areaFinPath = p->getAreaFin();
        
        // Se han definido las areas
        if(this->areaInicio != NULL){
            if(areaIniPath[0] < this->areaInicio[0])
                this->areaInicio[0] = areaIniPath[0];
            if(areaIniPath[1] < this->areaInicio[1])
                this->areaInicio[1] = areaIniPath[1];
            
            if(areaFinPath[0] > this->areaFin[0])
                this->areaFin[0] = areaFinPath[0];
            if(areaFinPath[1] > this->areaFin[1])
                this->areaFin[1] = areaFinPath[1];
        }
        else{ // No se ha definido el area
            this->areaInicio = areaIniPath;
            this->areaFin = areaFinPath;
        }
        
    }
    
    void iniciarVecPaths_aux(xml_node<>* nodo){
        if((string)nodo->name() == "path"){
            Path* p = new Path();
            for (xml_attribute<>* a = nodo->first_attribute(); a != NULL; a = a->next_attribute()) {
                string atrNombre = (string)a->name();
                
                for_each(atrNombre.begin(), atrNombre.end(), [](char & c){
                    c = ::tolower(c);
                });
                    
                p->setNodoXML(nodo);
                
                if(atrNombre == "id")
                    p->setId(a->value());
                else if(atrNombre == "opacity")
                    p->setOpacidad(a->value());
                else if(atrNombre == "d")
                    p->path(a->value());
                else if(atrNombre == "fill")
                    p->setColor(a->value());
                else if(atrNombre == "stroke")
                    p->setColor(a->value());
            }
            agregarPath(p);
        }
        
        // Recorre el primer hijo
        xml_node<>* nodoNuevo = nodo->first_node();
        if(nodoNuevo != NULL)
            iniciarVecPaths_aux(nodoNuevo);
        
        // Si no tiene primer hijo recorre a su siguiente hermano
        nodoNuevo = nodo->next_sibling(); 
        if(nodoNuevo != NULL)
            iniciarVecPaths_aux(nodoNuevo);
    }
    
    /*
     * Inicia el array de paths
     */
    void iniciarVecPaths(){
        iniciarVecPaths_aux(raiz.first_node());
    }
    
    /*
     * Pone en 0s el array entrante
     */
    void limpiarNumArray(double *arr, int tam){
        for (int i = 0; i < tam; i++)
            arr[i] = 0;
    }
    
    bool isArea(double x, double y){
        return x >= this->areaInicio[0] && y >= this->areaInicio[1] && x <= this->areaFin[0] && y <= this->areaFin[1];
    }
    
public:
    XML(string dir){
        // Pasa el string a char*
        this->nombre = dir.substr(0, dir.length()-4); // Guarda el nombre original del archivo
        
        setTam(raiz.first_node());
        
        iniciarVecPaths();
    }
    
    
    void animacion(double puntos[][2], int tamP, int colores[][3], int tamC, double angulo, int frames){
        vector<Path*> pathSeleccionados = seleccionar(puntos, tamP, colores, tamC);
        
        ruta(pathSeleccionados, angulo);
        frame(pathSeleccionados, frames);
        
    }
    
    void imprimir(){
        xml_node<>* nodo = raiz.first_node();
        imprimir(nodo);
    }
    
    /*
     *  Imprime el Ancho y el Alto del xml(svg)
     */
    void imprimirTam(){
        cout << "Ancho: " << this->ancho;
        cout << "\tAncho: " << this->alto << endl;
    }
    
    void imprimirPaths(){
        vector<Path*>::iterator fin = this->paths.end();
        for(vector<Path*>::iterator it = this->paths.begin(); it != fin; ++it)
        {
            ((Path*)*it)->imprimir();
        }
    }
    
    void imprimirArea(){
        cout << this->areaInicio[0] << "," << this->areaInicio[1] 
             << ";" << this->areaFin[0] << "," << this->areaFin[1] << endl;
    }
    
    void imprimirXML(){
        cout << *raiz.first_node() << endl;
    }
};

/*
 * 
 */
int main(int argc, char** argv) {
    string nombre = "wifi.svg";
    char* arr = strToChar(nombre);
    file<> file(arr);
    
    raiz.parse<0>(file.data()); // Parsea el xml a un arbol DOM
    
    XML* archivoXML = new XML(nombre);
    
    
    double puntos[][2] = {{0,0},{100,100},{1000,1000},{300,350},{3000,3500}};
    int tamP = (sizeof(puntos) / sizeof(puntos[0]));
    int colores[][3] = {{0,0,0}};
    int tamC = (sizeof(colores) / sizeof(colores[0]));
    
    double angulo = -35;
    int frames = 30;
    
    archivoXML->animacion(puntos, tamP, colores, tamC, angulo, frames);
    
    return 0;
}

