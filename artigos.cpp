#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <list>

using namespace std;

struct artigo
{
    vector<string> autores; //.size()
    int tipo; //periodico/conferencia
    int natureza; //completo/extendido/resumo
    int qualis; //A1/A2/B1/B2/B3/B4/B5/-
    string valor; //string exibida na listagem
};

struct pesquisador
{
    string nome;
    list<artigo>::iterator artigos; //só use antes de fazer odenações, ou seja, para carregar apenas os artigos de um pesquisador
};

struct dados
{
    vector<pesquisador> pesquisadores;
    list<artigo> artigos;
};

void escreveString(ofstream& arq,string s)
{
    int tam=s.size();
    arq.write((char*)&tam,sizeof(int));
    arq << s;
}

string leString(ifstream& arq)
{
    int tam;
    arq.read((char*)&tam,sizeof(int));
    char* buffer = new char[tam];
    arq.read(buffer,tam);
    string str(buffer,tam);
    return str;
}

void escrevePesquisador(pesquisador p,int numartigos)
{
    ofstream arq("artigos.bin",ios::binary|ios::app);
    escreveString(arq,p.nome);
    arq.write((char*)&numartigos,sizeof(int));
    list<artigo>::iterator it=p.artigos;
    for(int i=0;i<numartigos;i++,it++)
    {
        int tam=it->autores.size();
        arq.write((char*)&tam,sizeof(int));
        for(int j=0;j<tam;++j)
            escreveString(arq,it->autores[j]);
        arq.write((char*)&(it->tipo),sizeof(int));
        arq.write((char*)&(it->natureza),sizeof(int));
        arq.write((char*)&(it->qualis),sizeof(int));
        escreveString(arq,it->valor);
    }
}

dados leDados()
{
    ifstream arq("artigos.bin",ios::binary);
    dados d;
    while(!arq.eof())
    {
        int numartigos;
        pesquisador p;
        p.nome=leString(arq);
        arq.read((char*)&numartigos,sizeof(int));
        for(int i=0;i<numartigos;i++)
        {
            artigo novo;
            int tam;
            arq.read((char*)&tam,sizeof(int));
            for(int j=0;j<tam;++j)
                novo.autores.push_back(leString(arq));
            arq.read((char*)&(novo.tipo),sizeof(int));
            arq.read((char*)&(novo.natureza),sizeof(int));
            arq.read((char*)&(novo.qualis),sizeof(int));
            novo.valor=leString(arq);
            d.artigos.push_back(novo);
            if(i==0)
                p.artigos=(d.artigos.end()--);
        }
    }
    return d;
}

void leXML(string xmlpath,dados& d)
{
pesquisador pnovo;
    int numartigos=0;
    CMarkup xml;
    if (xml.Load(xmlpath)){
       
       xml.ResetPos();
       xml.FindElem("CURRICULO-VITAE");
       xml.IntoElem();
       xml.FindElem("DADOS-GERAIS");
       
       pnovo.nome = xml.GetAttrib("NOME-COMPLETO");
       
       xml.FindElem("PRODUCAO-BIBLIOGRAFICA");
       xml.IntoElem();
       
       xml.FindElem("TRABALHOS-EM-EVENTOS");
       xml.IntoElem();

       while(xml.FindElem("TRABALHO-EM-EVENTOS")){ //<TRABALHOS-EM-EVENTOS>
           artigo anovo;
           xml.SavePos();
           xml.IntoElem();
           xml.FindElem("DADOS-BASICOS-DO-TRABALHO");
           numartigos++;
           anovo.valor = xml.GetAttrib("TITULO-DO-TRABALHO");
           anovo.tipo = 0;
           anovo.qualis = 0;//tem que pegar do arquivo do moodle
           if (xml.GetAttrib("NATUREZA").compare("COMPLETO")==0)
              anovo.natureza = 0;
           else if (xml.GetAttrib("NATUREZA").compare("EXTENDIDO")==0)
              anovo.natureza = 1;
           else if (xml.GetAttrib("NATUREZA").compare("RESUMO")==0)
              anovo.natureza = 2;
           while(xml.FindElem("AUTORES")) //Autores do artigo
                anovo.autores.push_back(xml.GetAttrib("NOME-COMPLETO-DO-AUTOR"));
           d.artigos.push_back(anovo);
           pnovo.artigos=(d.artigos.end()--);
           xml.RestorePos();
       }
       
       xml.OutOfElem();
       xml.FindElem("ARTIGOS-PUBLICADOS");
       xml.IntoElem();

       while(xml.FindElem("ARTIGO-PUBLICADO")){ //<ARTIGOS-PUBLICADOS>
           artigo anovo;
           xml.SavePos();
           xml.IntoElem();
           xml.FindElem("DADOS-BASICOS-DO-ARTIGO");
           numartigos++;
           anovo.valor = xml.GetAttrib("TITULO-DO-ARTIGO"); 
           anovo.tipo = 1;
           anovo.qualis = 0;//tem que pegar do arquivo do moodle
           if (xml.GetAttrib("NATUREZA").compare("COMPLETO")==0)
               anovo.natureza = 0;
           else if (xml.GetAttrib("NATUREZA").compare("EXTENDIDO")==0)
               anovo.natureza = 1;
           else if (xml.GetAttrib("NATUREZA").compare("RESUMO")==0)
               anovo.natureza = 2;
           while(xml.FindElem("AUTORES")) //Autores do artigo
               anovo.autores.push_back(xml.GetAttrib("NOME-COMPLETO-DO-AUTOR"));
           d.artigos.push_back(anovo);
           pnovo.artigos=(d.artigos.end()--);
           xml.RestorePos();
       }
       
       d.pesquisadores.push_back(pnovo);
       //escrevePesquisador(pnovo,numartigos);
    }else{
          printf("Problema na leitura do xml");
    }
}

int main()
{
    dados d=leDados();
    leXML("curriculo.xml",d);
    cout << "Antes: " << d.pesquisadores[0].artigos->valor << endl;
    d=leDados();
    cout << "Depois: " << d.pesquisadores[0].artigos->valor << endl;
    /*
    Para ordenar, use d.artigos.sort(comp);
        bool comp(artigo a,artigo b) {return a<b};
    Para filtrar, use d.artigos.erase(it pos); ou d.artigos.remove_if(cond);
        bool cond(artigo a) {return a.remove};
    */

    return 0;
}
