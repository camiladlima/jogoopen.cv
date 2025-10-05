#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>
#include <vector>
#include <random>
#include <fstream>
#include <ctime>
#include <cstdlib>

using namespace cv;
using namespace std;

// Classe para gerenciar áudio com arquivos MP3
class AudioManager {
private:
    bool audioEnabled;
    
public:
    AudioManager() : audioEnabled(true) {}
    
    void playBackgroundMusic() {
        if (!audioEnabled) return;
        #ifdef _WIN32
            system("start /B 8bit-music-for-game-68698.mp3");
        #else
            system("pkill -f 'mpg123' 2>/dev/null");
            system("mpg123 -q 8bit-music-for-game-68698.mp3 --loop -1 & 2>/dev/null");
        #endif
    }
    
    void stopBackgroundMusic() {
        #ifdef _WIN32
            system("taskkill /f /im mp3.exe 2>nul");
        #else
            system("pkill -f 'mpg123' 2>/dev/null");
        #endif
    }
    
    void playStartSound() {
        if (!audioEnabled) return;
        #ifdef _WIN32
            system("start /B start.mp3");
        #else
            system("mpg123 -q start.mp3 & 2>/dev/null");
        #endif
    }
    
    void playHeartLostSound() {
        if (!audioEnabled) return;
        #ifdef _WIN32
            system("start /B falled-sound-effect-278635.mp3");
        #else
            system("mpg123 -q falled-sound-effect-278635.mp3 & 2>/dev/null");
        #endif
    }
    
    void playGameOverSound() {
        if (!audioEnabled) return;
        #ifdef _WIN32
            system("start /B  videogame-death-sound-43894.mp3");
        #else
            system("mpg123 -q videogame-death-sound-43894.mp3 & 2>/dev/null");
        #endif
    }
    
    void playCollectSound() {
        if (!audioEnabled) return;
        #ifdef _WIN32
            system("start /B  game-start-317318.mp3");
        #else
            system("mpg123 -q  game-start-317318.mp3 & 2>/dev/null");
        #endif
    }
    
    void disableAudio() {
        audioEnabled = false;
        stopBackgroundMusic();
    }
    
    bool isAudioEnabled() {
        return audioEnabled;
    }
};

// Classe para representar objetos caindo com imagens
class FallingObject {
public:
    Point2f position;
    float speed;
    float size;
    Mat image;
    bool image_loaded;
    
    FallingObject(float start_x, float start_speed, float obj_size, const vector<Mat>& object_images) {
        position = Point2f(start_x, -50);
        speed = start_speed;
        size = obj_size;
        
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(0, object_images.size() - 1);
        
        int img_index = dis(gen);
        if (!object_images.empty() && !object_images[img_index].empty()) {
            object_images[img_index].copyTo(image);
            image_loaded = true;
        } else {
            image_loaded = false;
        }
    }
    
    void update() {
        position.y += speed;
    }
    
    void draw(Mat& frame) {
        if (image_loaded && !image.empty()) {
            int new_width = size * 2;
            int new_height = size * 2;
            Mat resized_img;
            resize(image, resized_img, Size(new_width, new_height));
            
            int x = position.x - new_width/2;
            int y = position.y - new_height/2;
            
            if (x >= 0 && y >= 0 && x + new_width <= frame.cols && y + new_height <= frame.rows) {
                Rect roi(x, y, new_width, new_height);
                resized_img.copyTo(frame(roi));
            }
        } else {
            circle(frame, position, size, Scalar(255, 0, 0), -1);
            circle(frame, position, size, Scalar(255, 255, 255), 2);
        }
    }
    
    bool isOutOfScreen(int height) {
        return position.y > height + 50;
    }
};

// Função para carregar imagens dos objetos
vector<Mat> loadObjectImages() {
    vector<Mat> images;
    vector<string> image_paths = {
        "Statue2_shadow1.png",
        "Ship1_shadow1.png", 
        "Statue1_shadow1.png",
        "Crab_shadow1.png",
        "Dragon_bones_shadow1.png"
    };
    
    for (const auto& path : image_paths) {
        Mat img = imread(path);
        if (!img.empty()) {
            images.push_back(img);
            cout << "✅ Carregou: " << path << endl;
        } else {
            cout << "❌ Não encontrou: " << path << endl;
        }
    }
    
    if (images.empty()) {
        cout << "⚠️  Criando imagens padrão..." << endl;
        
        Mat star(60, 60, CV_8UC3, Scalar(0, 0, 0));
        for (int i = 0; i < 60; i++) {
            for (int j = 0; j < 60; j++) {
                double angle = atan2(i-30, j-30);
                double distance = sqrt(pow(i-30, 2) + pow(j-30, 2));
                if (distance < 25 && distance > 20 && (int(angle * 5) % 2 == 0)) {
                    star.at<Vec3b>(i, j) = Vec3b(0, 255, 255);
                } else if (distance < 15) {
                    star.at<Vec3b>(i, j) = Vec3b(0, 255, 255);
                }
            }
        }
        images.push_back(star);
        
        Mat heart(60, 60, CV_8UC3, Scalar(0, 0, 0));
        for (int i = 0; i < 60; i++) {
            for (int j = 0; j < 60; j++) {
                double x = (j - 30) / 15.0;
                double y = -(i - 30) / 15.0;
                double heart_eq = pow(x, 2) + pow(y - sqrt(abs(x)), 2);
                if (heart_eq < 1.5) {
                    heart.at<Vec3b>(i, j) = Vec3b(0, 0, 255);
                }
            }
        }
        images.push_back(heart);
        
        Mat smiley(60, 60, CV_8UC3, Scalar(0, 0, 0));
        circle(smiley, Point(30, 30), 25, Scalar(0, 255, 255), -1);
        circle(smiley, Point(20, 20), 5, Scalar(0, 0, 0), -1);
        circle(smiley, Point(40, 20), 5, Scalar(0, 0, 0), -1);
        ellipse(smiley, Point(30, 35), Size(10, 5), 0, 0, 180, Scalar(0, 0, 0), 3);
        images.push_back(smiley);
    }
    
    return images;
}

// Função para criar botão personalizado
Mat criarBotao(int largura, int altura, Scalar cor_fundo, const string& texto, bool hover = false) {
    Mat botao(altura, largura, CV_8UC3, cor_fundo);
    
    rectangle(botao, Rect(0, 0, largura, altura), Scalar(255, 255, 255), 3);
    
    if (hover) {
        for (int i = 0; i < altura; i++) {
            for (int j = 0; j < largura; j++) {
                double distance = sqrt(pow(j - largura/2, 2) + pow(i - altura/2, 2));
                double intensity = max(0.0, 1.0 - distance / (largura/2));
                
                Vec3b original = botao.at<Vec3b>(i, j);
                botao.at<Vec3b>(i, j) = Vec3b(
                    min(255, int(original[0] + 50 * intensity)),
                    min(255, int(original[1] + 50 * intensity)),
                    min(255, int(original[2] + 50 * intensity))
                );
            }
        }
        rectangle(botao, Rect(2, 2, largura-4, altura-4), Scalar(255, 255, 255), 2);
    }
    
    int fontScale;
    int thickness = 2;
    
    if (texto.length() > 10) {
        fontScale = largura / 250;
    } else {
        fontScale = largura / 180;
    }
    
    Size text_size = getTextSize(texto, FONT_HERSHEY_DUPLEX, fontScale, thickness, 0);
    Point text_pos((largura - text_size.width) / 2, (altura + text_size.height) / 2);
    
    putText(botao, texto, text_pos + Point(2, 2), FONT_HERSHEY_DUPLEX, fontScale, Scalar(0, 0, 0), thickness);
    putText(botao, texto, text_pos, FONT_HERSHEY_DUPLEX, fontScale, Scalar(255, 255, 255), thickness);
    
    return botao;
}

// Função para mostrar tela de início
bool mostrarTelaInicio(Mat& background, int highscore, AudioManager& audio) {
    audio.playStartSound();
    
    Mat botao_normal = criarBotao(300, 100, Scalar(0, 100, 200), "JOGAR");
    Mat botao_hover = criarBotao(300, 100, Scalar(0, 150, 250), "JOGAR", true);

    Mat tela;
    if (background.cols != 640 || background.rows != 480) {
        resize(background, tela, Size(640, 480));
    } else {
        tela = background.clone();
    }

    int btn_x = tela.cols/2 - botao_normal.cols/2;
    int btn_y = tela.rows/2 - botao_normal.rows/2 + 50;

    namedWindow("Tela Inicial");
    bool clicado = false;
    Point mouse_pos(0, 0);
    
    setMouseCallback("Tela Inicial", [](int event, int x, int y, int flags, void* userdata) {
        auto* data = static_cast<pair<Point*, bool*>*>(userdata);
        data->first->x = x;
        data->first->y = y;
        
        if (event == EVENT_LBUTTONDOWN) {
            if (x >= 170 && x <= 470 && y >= 240 && y <= 340) {
                *(data->second) = true;
                cout << "🎮 Botão JOGAR clicado! Iniciando jogo..." << endl;
            }
        }
    }, new pair<Point*, bool*>(&mouse_pos, &clicado));

    cout << "🎮 Tela inicial - Clique em 'JOGAR' para começar!" << endl;

    while (!clicado) {
        Mat frame_display = tela.clone();

        bool hover = (mouse_pos.x >= 170 && mouse_pos.x <= 470 && 
                     mouse_pos.y >= 240 && mouse_pos.y <= 340);

        Mat btn_to_draw = hover ? botao_hover : botao_normal;
        
        Rect safe_roi(btn_x, btn_y, 
                     min(btn_to_draw.cols, frame_display.cols - btn_x),
                     min(btn_to_draw.rows, frame_display.rows - btn_y));
        
        if (safe_roi.width > 0 && safe_roi.height > 0) {
            btn_to_draw.copyTo(frame_display(safe_roi));
        }

        putText(frame_display, "CAPTURE OS OBJETOS", Point(120, 100), 
                FONT_HERSHEY_SIMPLEX, 1.2, Scalar(255, 255, 255), 3);
        putText(frame_display, "COM SEU ROSTO!", Point(160, 140), 
                FONT_HERSHEY_SIMPLEX, 1.2, Scalar(255, 255, 255), 3);
        
        putText(frame_display, "RECORDE: " + to_string(highscore), Point(250, 200), 
                FONT_HERSHEY_SIMPLEX, 0.8, Scalar(255, 255, 0), 2);
        
        if (hover) {
            putText(frame_display, "Clique para JOGAR!", Point(220, 400), 
                    FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 255, 0), 2);
        } else {
            putText(frame_display, "Use seu rosto para coletar os objetos que caem", Point(80, 400), 
                    FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255, 255, 255), 2);
        }

        imshow("Tela Inicial", frame_display);
        
        int key = waitKey(10);
        if (key == 27) {
            destroyWindow("Tela Inicial");
            return false;
        }
        
        if (key == 13) {
            clicado = true;
            cout << "🎮 Iniciando jogo com Enter!" << endl;
        }
    }

    destroyWindow("Tela Inicial");
    cout << "🚀 Iniciando jogo principal..." << endl;
    return true;
}

// Função para mostrar tela de game over
bool mostrarTelaGameOver(int score, int highscore, AudioManager& audio) {
    audio.playGameOverSound();
    
    Mat tela(480, 640, CV_8UC3, Scalar(50, 50, 80));
    
    Mat botao_normal = criarBotao(300, 80, Scalar(0, 100, 200), "JOGAR");
    Mat botao_hover = criarBotao(300, 80, Scalar(0, 150, 250), "JOGAR", true);
    Mat botao_sair_normal = criarBotao(200, 60, Scalar(0, 50, 100), "SAIR");
    Mat botao_sair_hover = criarBotao(200, 60, Scalar(0, 80, 150), "SAIR", true);

    int btn_jogar_x = tela.cols/2 - botao_normal.cols/2;
    int btn_jogar_y = tela.rows/2 + 20;
    int btn_sair_x = tela.cols/2 - botao_sair_normal.cols/2;
    int btn_sair_y = tela.rows/2 + 120;

    namedWindow("Fim de Jogo");
    bool jogar_novamente = false;
    bool sair = false;
    Point mouse_pos(0, 0);
    
    setMouseCallback("Fim de Jogo", [](int event, int x, int y, int flags, void* userdata) {
        auto* data = static_cast<tuple<Point*, bool*, bool*>*>(userdata);
        get<0>(*data)->x = x;
        get<0>(*data)->y = y;
        
        if (event == EVENT_LBUTTONDOWN) {
            if (x >= 170 && x <= 470 && y >= 260 && y <= 340) {
                *(get<1>(*data)) = true;
                cout << "🔄 Jogar novamente clicado!" << endl;
            }
            else if (x >= 220 && x <= 420 && y >= 360 && y <= 420) {
                *(get<2>(*data)) = true;
                cout << "👋 Sair clicado!" << endl;
            }
        }
    }, new tuple<Point*, bool*, bool*>(&mouse_pos, &jogar_novamente, &sair));

    cout << "🎮 Tela de Game Over - Escolha uma opção" << endl;

    while (!jogar_novamente && !sair) {
        Mat frame_display = tela.clone();

        bool hover_jogar = (mouse_pos.x >= 170 && mouse_pos.x <= 470 && 
                           mouse_pos.y >= 260 && mouse_pos.y <= 340);
        bool hover_sair = (mouse_pos.x >= 220 && mouse_pos.x <= 420 && 
                          mouse_pos.y >= 360 && mouse_pos.y <= 420);

        putText(frame_display, "FIM DE JOGO", Point(200, 100), 
                FONT_HERSHEY_SIMPLEX, 1.5, Scalar(0, 0, 255), 3);
        
        putText(frame_display, "SCORE: " + to_string(score), Point(250, 160), 
                FONT_HERSHEY_SIMPLEX, 1.0, Scalar(255, 255, 255), 2);
        
        putText(frame_display, "RECORDE: " + to_string(highscore), Point(230, 200), 
                FONT_HERSHEY_SIMPLEX, 1.0, Scalar(255, 255, 0), 2);

        Mat btn_jogar = hover_jogar ? botao_hover : botao_normal;
        Mat btn_sair = hover_sair ? botao_sair_hover : botao_sair_normal;
        
        btn_jogar.copyTo(frame_display(Rect(btn_jogar_x, btn_jogar_y, btn_jogar.cols, btn_jogar.rows)));
        btn_sair.copyTo(frame_display(Rect(btn_sair_x, btn_sair_y, btn_sair.cols, btn_sair.rows)));

        if (hover_jogar) {
            putText(frame_display, "Clique para jogar novamente", Point(210, 420), 
                    FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255, 255, 0), 2);
        } else if (hover_sair) {
            putText(frame_display, "Clique para sair do jogo", Point(230, 450), 
                    FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255, 255, 0), 2);
        }

        imshow("Fim de Jogo", frame_display);
        
        int key = waitKey(10);
        if (key == 27) {
            sair = true;
        }
    }

    destroyWindow("Fim de Jogo");
    
    if (jogar_novamente) {
        cout << "🔄 Reiniciando jogo..." << endl;
        return true;
    } else {
        cout << "👋 Saindo do jogo..." << endl;
        return false;
    }
}

// Função para desenhar os corações (vidas)
void drawHearts(Mat& frame, int hearts) {
    int heartSize = 30;
    int spacing = 10;
    int startX = frame.cols - (heartSize * 3 + spacing * 2) - 10;
    int startY = 10;
    
    for (int i = 0; i < 3; i++) {
        Scalar color = (i < hearts) ? Scalar(0, 0, 255) : Scalar(50, 50, 50);
        
        Point center(startX + i * (heartSize + spacing) + heartSize/2, startY + heartSize/2);
        
        circle(frame, Point(center.x - heartSize/4, center.y - heartSize/4), heartSize/4, color, -1);
        circle(frame, Point(center.x + heartSize/4, center.y - heartSize/4), heartSize/4, color, -1);
        
        vector<Point> triangle = {
            Point(center.x - heartSize/2, center.y - heartSize/8),
            Point(center.x + heartSize/2, center.y - heartSize/8),
            Point(center.x, center.y + heartSize/2)
        };
        fillConvexPoly(frame, triangle, color);
    }
}

int main() {
    AudioManager audio;
    
    // Testa se os programas de áudio estão disponíveis
    #ifdef _WIN32
        if (system("where ffplay >nul 2>nul") != 0) {
            cout << "⚠️  ffplay não encontrado. O jogo continuará sem áudio." << endl;
            audio.disableAudio();
        } else {
            cout << "🎵 Sistema de áudio ativado!" << endl;
        }
    #else
        if (system("which mpg123 > /dev/null 2>&1") != 0) {
            cout << "⚠️  mpg123 não encontrado. O jogo continuará sem áudio." << endl;
            audio.disableAudio();
        } else {
            cout << "🎵 Sistema de áudio ativado!" << endl;
        }
    #endif

    int highscore = 0;
    ifstream recorde_file("recorde.txt");
    if (recorde_file.is_open()) {
        recorde_file >> highscore;
        recorde_file.close();
    }

    Mat background = imread("orig_big.png");
    if (background.empty()) {
        background = Mat(480, 640, CV_8UC3);
        for (int i = 0; i < background.rows; i++) {
            for (int j = 0; j < background.cols; j++) {
                background.at<Vec3b>(i, j) = Vec3b(
                    j * 255 / background.cols,
                    i * 255 / background.rows,  
                    128
                );
            }
        }
        cout << "⚠️  Usando fundo padrão. Coloque uma imagem 'orig_big.png' no mesmo diretório." << endl;
    }

    bool jogando = true;
    
    while (jogando) {
        if (!mostrarTelaInicio(background, highscore, audio)) {
            cout << "👋 Jogo fechado pelo usuário." << endl;
            break;
        }

        audio.playBackgroundMusic();

        VideoCapture cap(0);
        if (!cap.isOpened()) {
            cerr << "❌ Erro: não consegui abrir a câmera!" << endl;
            return -1;
        }

        CascadeClassifier face_cascade;
        if (!face_cascade.load("/usr/share/opencv4/haarcascades/haarcascade_frontalface_default.xml")) {
            cerr << "❌ Erro: não consegui carregar o classificador de faces!" << endl;
            return -1;
        }

        Mat frame;
        vector<FallingObject> objects;
        vector<Mat> object_images = loadObjectImages();
        cout << "📸 " << object_images.size() << " imagens de objetos carregadas" << endl;

        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<> pos_dis(0, 640);
        uniform_real_distribution<> speed_dis(2, 8);
        uniform_real_distribution<> size_dis(20, 40);

        int frame_count = 0;
        int score = 0;
        int hearts = 3;
        bool game_over = false;
        int spawn_timer = 0;
        int spawn_interval = 60;
        int difficulty_timer = 0;
        int last_hearts = hearts;

        cout << "🎮 Jogo iniciado! Você tem 3 vidas." << endl;

        while (true) {
            cap >> frame;
            if (frame.empty()) break;

            flip(frame, frame, 1);
            Mat resized_background;
            resize(background, resized_background, frame.size());

            Mat gray, mask;
            cvtColor(frame, gray, COLOR_BGR2GRAY);
            equalizeHist(gray, gray);

            vector<Rect> faces;
            face_cascade.detectMultiScale(gray, faces, 1.1, 3, 0, Size(80, 80));

            mask = Mat::zeros(frame.size(), CV_8UC1);
            for (size_t i = 0; i < faces.size(); i++) {
                Point center(faces[i].x + faces[i].width/2, faces[i].y + faces[i].height/2);
                int radius = cvRound((faces[i].width + faces[i].height) * 0.25);
                circle(mask, center, radius, Scalar(255), -1);
            }

            Mat result;
            resized_background.copyTo(result);
            frame.copyTo(result, mask);

            frame_count++;
            spawn_timer++;
            difficulty_timer++;
            
            if (difficulty_timer > 300) {
                spawn_interval = max(20, spawn_interval - 5);
                difficulty_timer = 0;
                cout << "📈 Dificuldade aumentada! Spawn interval: " << spawn_interval << endl;
            }
            
            if (spawn_timer >= spawn_interval && !game_over) {
                objects.push_back(FallingObject(
                    pos_dis(gen), 
                    speed_dis(gen), 
                    size_dis(gen),
                    object_images
                ));
                spawn_timer = 0;
            }

            for (auto it = objects.begin(); it != objects.end();) {
                it->update();
                
                bool collision = false;
                for (size_t i = 0; i < faces.size() && !collision; i++) {
                    Point2f face_center(
                        faces[i].x + faces[i].width/2.0f, 
                        faces[i].y + faces[i].height/2.0f
                    );
                    float face_radius = (faces[i].width + faces[i].height) * 0.25f;
                    
                    float dx = face_center.x - it->position.x;
                    float dy = face_center.y - it->position.y;
                    double distance = sqrt(dx*dx + dy*dy);
                    
                    if (distance < (face_radius + it->size)) {
                        collision = true;
                        score += 10;
                        audio.playCollectSound();
                    }
                }
                
                if (collision) {
                    it = objects.erase(it);
                } else if (it->isOutOfScreen(result.rows)) {
                    if (!game_over) {
                        hearts--;
                        cout << "💔 Vida perdida! Vidas restantes: " << hearts << endl;
                        
                        // CORREÇÃO AQUI: usa o método da classe AudioManager
                        if (audio.isAudioEnabled() && hearts < last_hearts) {
                            audio.playHeartLostSound();
                            last_hearts = hearts;
                        }
                        
                        if (hearts <= 0) {
                            game_over = true;
                            cout << "🎮 FIM DE JOGO! Score final: " << score << endl;
                            audio.stopBackgroundMusic();
                            break;
                        }
                    }
                    it = objects.erase(it);
                } else {
                    it->draw(result);
                    ++it;
                }
            }

            if (game_over) {
                break;
            }

            putText(result, "Score: " + to_string(score), Point(10, 30), 
                    FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 255, 255), 2);
            drawHearts(result, hearts);
            putText(result, "Colete os objetos com seu rosto!", Point(10, result.rows - 10), 
                    FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);

            imshow("Webcam com Detecao Facial e Objetos", result);

            char c = (char)waitKey(10);
            if (c == 27) break;
        }

        destroyWindow("Webcam com Detecao Facial e Objetos");
        cap.release();

        if (score > highscore) {
            highscore = score;
            ofstream recorde_file("recorde.txt");
            recorde_file << score;
            recorde_file.close();
            cout << "🏆 Novo recorde: " << score << endl;
        }

        if (!mostrarTelaGameOver(score, highscore, audio)) {
            jogando = false;
        } else {
            cout << "🔄 Reiniciando jogo..." << endl;
        }
    }

    audio.stopBackgroundMusic();
    cout << "👋 Jogo encerrado!" << endl;
    return 0;
}
