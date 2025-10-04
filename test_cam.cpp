#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>
#include <vector>
#include <random>

using namespace cv;
using namespace std;

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
        
        // Escolhe uma imagem aleatória da lista
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
            // Redimensiona a imagem para o tamanho desejado
            int new_width = size * 2;
            int new_height = size * 2;
            Mat resized_img;
            resize(image, resized_img, Size(new_width, new_height));
            
            // Calcula a posição para centralizar a imagem
            int x = position.x - new_width/2;
            int y = position.y - new_height/2;
            
            // Verifica se está dentro dos limites da tela
            if (x >= 0 && y >= 0 && x + new_width <= frame.cols && y + new_height <= frame.rows) {
                // Cria ROI (Region of Interest) e copia a imagem
                Rect roi(x, y, new_width, new_height);
                resized_img.copyTo(frame(roi));
            }
        } else {
            // Fallback: desenha um círculo se a imagem não carregou
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
    
    // Se não carregou nenhuma imagem, cria algumas imagens simples
    if (images.empty()) {
        cout << "⚠️  Criando imagens padrão..." << endl;
        
        // Imagem 1: Estrela amarela
        Mat star(60, 60, CV_8UC3, Scalar(0, 0, 0));
        for (int i = 0; i < 60; i++) {
            for (int j = 0; j < 60; j++) {
                double angle = atan2(i-30, j-30);
                double distance = sqrt(pow(i-30, 2) + pow(j-30, 2));
                if (distance < 25 && distance > 20 && (int(angle * 5) % 2 == 0)) {
                    star.at<Vec3b>(i, j) = Vec3b(0, 255, 255); // Amarelo
                } else if (distance < 15) {
                    star.at<Vec3b>(i, j) = Vec3b(0, 255, 255); // Amarelo
                }
            }
        }
        images.push_back(star);
        
        // Imagem 2: Coração vermelho
        Mat heart(60, 60, CV_8UC3, Scalar(0, 0, 0));
        for (int i = 0; i < 60; i++) {
            for (int j = 0; j < 60; j++) {
                double x = (j - 30) / 15.0;
                double y = -(i - 30) / 15.0;
                double heart_eq = pow(x, 2) + pow(y - sqrt(abs(x)), 2);
                if (heart_eq < 1.5) {
                    heart.at<Vec3b>(i, j) = Vec3b(0, 0, 255); // Vermelho
                }
            }
        }
        images.push_back(heart);
        
        // Imagem 3: Smiley feliz
        Mat smiley(60, 60, CV_8UC3, Scalar(0, 0, 0));
        circle(smiley, Point(30, 30), 25, Scalar(0, 255, 255), -1); // Face amarela
        circle(smiley, Point(20, 20), 5, Scalar(0, 0, 0), -1); // Olho esquerdo
        circle(smiley, Point(40, 20), 5, Scalar(0, 0, 0), -1); // Olho direito
        ellipse(smiley, Point(30, 35), Size(10, 5), 0, 0, 180, Scalar(0, 0, 0), 3); // Boca
        images.push_back(smiley);
    }
    
    return images;
}

int main() {
    VideoCapture cap(0);
    if (!cap.isOpened()) {
        cerr << "❌ Erro: não consegui abrir a câmera!" << endl;
        return -1;
    }

    // Carrega o classificador para detecção facial
    CascadeClassifier face_cascade;
    if (!face_cascade.load("/usr/share/opencv4/haarcascades/haarcascade_frontalface_default.xml")) {
        cerr << "❌ Erro: não consegui carregar o classificador de faces!" << endl;
        return -1;
    }

    Mat frame, background;
    vector<FallingObject> objects;
    
    // Carrega imagens dos objetos
    vector<Mat> object_images = loadObjectImages();
    cout << "📸 " << object_images.size() << " imagens de objetos carregadas" << endl;
    
    // Carrega imagem de fundo
    background = imread("orig_big.png");
    if (background.empty()) {
        // Se não encontrar a imagem, cria um fundo gradiente
        background = Mat(480, 640, CV_8UC3);
        for (int i = 0; i < background.rows; i++) {
            for (int j = 0; j < background.cols; j++) {
                background.at<Vec3b>(i, j) = Vec3b(
                    j * 255 / background.cols,  // B
                    i * 255 / background.rows,  // G  
                    128                         // R
                );
            }
        }
        cout << "⚠️  Usando fundo padrão. Coloque uma imagem 'fundo.jpg' no mesmo diretório." << endl;
    }

    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> pos_dis(0, 640);
    uniform_real_distribution<> speed_dis(2, 8);
    uniform_real_distribution<> size_dis(20, 40); // Tamanho maior para as imagens

    int frame_count = 0;
    double fps = 0;
    int score = 0;
    time_t start_time = time(0);

    cout << "🎮 Controles:" << endl;
    cout << "   - ESC: Sair" << endl;
    cout << "   - Espaço: Adicionar mais objetos" << endl;

    while (true) {
        cap >> frame;
        if (frame.empty()) break;

        // 🔄 Espelha horizontalmente
        flip(frame, frame, 1);

        // Redimensiona o fundo para o tamanho do frame
        Mat resized_background;
        resize(background, resized_background, frame.size());

        // Cria uma máscara para a detecção facial
        Mat gray, mask;
        cvtColor(frame, gray, COLOR_BGR2GRAY);
        equalizeHist(gray, gray);

        // Detecta faces
        vector<Rect> faces;
        face_cascade.detectMultiScale(gray, faces, 1.1, 3, 0, Size(80, 80));

        // Cria máscara onde as faces serão mostradas
        mask = Mat::zeros(frame.size(), CV_8UC1);

        // Desenha círculos nas faces detectadas
        for (size_t i = 0; i < faces.size(); i++) {
            Point center(faces[i].x + faces[i].width/2, faces[i].y + faces[i].height/2);
            int radius = cvRound((faces[i].width + faces[i].height) * 0.25);
            circle(mask, center, radius, Scalar(255), -1);
        }

        // Aplica o fundo, mantendo as faces da webcam
        Mat result;
        resized_background.copyTo(result);
        frame.copyTo(result, mask);

        // Atualiza e desenha objetos caindo
        frame_count++;
        
        // Adiciona novos objetos aleatoriamente
        if (frame_count % 30 == 0 || objects.size() < 3) {
            objects.push_back(FallingObject(
                pos_dis(gen), 
                speed_dis(gen), 
                size_dis(gen),
                object_images
            ));
        }

        // Verifica colisões e atualiza objetos
        for (auto it = objects.begin(); it != objects.end();) {
            it->update();
            
            // Verifica colisão com faces
            bool collision = false;
            for (size_t i = 0; i < faces.size() && !collision; i++) {
                Point2f face_center(
                    faces[i].x + faces[i].width/2.0f, 
                    faces[i].y + faces[i].height/2.0f
                );
                float face_radius = (faces[i].width + faces[i].height) * 0.25f;
                
                // Calcula distância entre centros
                float dx = face_center.x - it->position.x;
                float dy = face_center.y - it->position.y;
                double distance = sqrt(dx*dx + dy*dy);
                
                if (distance < (face_radius + it->size)) {
                    collision = true;
                    score += 10;
                }
            }
            
            if (collision || it->isOutOfScreen(result.rows)) {
                it = objects.erase(it);
            } else {
                it->draw(result);
                ++it;
            }
        }

        // Calcula FPS
        if (frame_count % 30 == 0) {
            time_t current_time = time(0);
            fps = 30.0 / (current_time - start_time);
            start_time = current_time;
        }

        // Adiciona HUD (informações na tela)
        putText(result, "FPS: " + to_string((int)fps), Point(10, 30), 
                FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 255, 255), 2);
        putText(result, "Score: " + to_string(score), Point(10, 60), 
                FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 255, 255), 2);
        putText(result, "Faces: " + to_string(faces.size()), Point(10, 90), 
                FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 255, 255), 2);
        putText(result, "Objetos: " + to_string(objects.size()), Point(10, 120), 
                FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 255, 255), 2);

        // Instruções
        putText(result, "ESC: Sair | ESPACO: +Objetos", Point(10, result.rows - 10), 
                FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);

        // Mostra o resultado
        imshow("Webcam com Detecao Facial e Objetos", result);

        // Controles
        char c = (char)waitKey(10);
        if (c == 27) break; // ESC
        if (c == 32) { // Espaço - adiciona mais objetos
            for (int i = 0; i < 3; i++) {
                objects.push_back(FallingObject(
                    pos_dis(gen), 
                    speed_dis(gen), 
                    size_dis(gen),
                    object_images
                ));
            }
        }
    }

    cout << "🎯 Score final: " << score << endl;
    return 0;
}
