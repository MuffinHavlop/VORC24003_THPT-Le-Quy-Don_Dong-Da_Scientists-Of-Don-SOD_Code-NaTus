// Import headers của các thư viện
#include <PS2X_lib.h>
#include <Adafruit_PWMServoDriver.h>
#include <Wire.h>

// Set up các chân tín hiệu của tay cầm PS2
#define PS2_DAT 12
#define PS2_CMD 13
#define PS2_CLK 14
#define PS2_ATT 15

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

void setup()              // Hàm set up chạy khởi tạo một lần khi khởi động mạch
{                         
    Serial.begin(115200); // Bật serial monitor ở tốc độ 115200

    Serial.println("SOD's Banhmi Connected To PC. Proceeding to connect PS2 Controller");

    while (ps2.config_gamepad(PS2_CLK, PS2_CMD, PS2_ATT, PS2_DAT) != 0)
    {
        Serial.println("Err: PS2 Controller Not Connected");
        delay(500);
    } // Check xem có chân tín hiệu nào của điều khiển nhận đc hay ko

    Serial.println("Controller works normally");

    pwm.begin();                          // Khởi tạo xung pwm
    pwm.setOscillatorFrequency(27000000); // Đặt tần số xung trong PCA9685 là 27000000 (27MHz) (27 triệu)
    pwm.setPWMFreq(50);                   // Đặt tần số giao động trên chân tối đa 50Hz (Pulse Width Modulation)
    bool INTAKE = false;                  // Đặt 4 biến cần thiết của hai cơ chế intake/outtake và Push
    bool OUTTAE = false;
    bool OFF_IO = false;
    bool ON_OFF_P = false; 
}

void loop()
{
    PS2.read_gamepad();
    //Chuỗi lệnh điều khiển bánh trái
    if (ps2x.Analog(PSS_LY) > 0)
    {
        pwm.setPWM(12, 0, ps2x.Analog(PSS_LY)*16); //ps2x.Analog(PSS_LY)*16 có nghĩa là sẽ lấy giá trị của trục LY từ 0->255 và nhân với 16. Ví dụ: 255*16 = 4080, cho giá trị động cơ gần tối đa
        pwm.setPWM(13, 0, 0);                      //Thiết kế này có điểm tốt hơn cũ là driver sẽ chủ động được việc điều khiển tốc độ động cơ thay vì chỉ có một tốc độ tối đa như cũ
    }
    else if (ps2x.Analog(PSS_LY) < 0)
    {
        pwm.setPWM(12, 0, 0);
        pwm.setPWM(13, 0, -ps2x.Analog(PSS_LY)*16);    //Dấu trừ của -ps2x.Analog(PSS_LY)*16 là để biến giá trị của trục OY thành giá trị dương (vì LY đang là âm) tại vì PWM không chạy được ở giá trị âm
    }
    else
    {
        pwm.setPWM(13, 0, 0);
        pwm.setPWM(12, 0, 0);
    }

    //Chuỗi lệnh điều khiển bánh phải
    if (ps2x.Analog(PSS_RY) > 0)                            //ps2x.Analog(PSS_RY)*16 có nghĩa là sẽ lấy giá trị của trục LY từ 0->255 và nhân với 16. Ví dụ: 255*16 = 4080, cho giá trị động cơ gần tối đa
    {                                                       //Thiết kế này có điểm tốt hơn cũ là driver sẽ chủ động được việc điều khiển tốc độ động cơ thay vì chỉ có một tốc độ tối đa như cũ
        pwm.setPWM(10, 0, 0);
        pwm.setPWM(11, 0, ps2x.Analog(PSS.RY)*16);
    }
    else if (ps2x.Analog(PSS_RY) < 0)
    {
        pwm.setPWM(10, 0, -ps2x.Analog(PSS.RY)*16);        //Dấu trừ của -ps2x.Analog(PSS_RY)*16 là để biến giá trị của trục OY thành giá trị dương (vì LY đang là âm) tại vì PWM không chạy được ở giá trị âm
        pwm.setPWM(11, 0, 0);
    }
    else
    {
        pwm.setPWM(10, 0, 0);        //Tắt động cơ khi Joystick bằng 0
        pwm.setPWM(11, 0, 0);
    }

    //Chuỗi lệnh để bật cơ chế "INTAKE"
    if (ps2x.Button(PSB_PAD_UP))
    {
        bool INTAKE = true;     //Ấn vào nút PAD_UP sẽ kích hoạt cơ chế INTAKE (Lấy bóng) bằng cách cho giá trị boolean INTAKE = true
        bool OUTTAKE = false; 
        bool OFF_IO = false;  
    }
    if (INTAKE == true)        //Nếu INTAKE = true sẽ chạy động cơ chiều xuôi
            {
                pwm.setPWM(8, 0, 4096);
                pwm.setPWM(9, 0, 0);
            }
    //Chuỗi lệnh để bật cơ chế "OUTTAKE"
    if (ps2x.Button(PSB_PAD_DOWN))
    {
        bool INTAKE = false;        
        bool OUTTAKE = true;      //Ấn vào nút PAD_DOWN sẽ kích hoạt cơ chế OUTTAKE (Thả bóng) bằng cách cho giá trị boolean OUTTAKE = false
        bool OFF_IO = false;    
    }
    if (OUTTAKE == true)    //Nếu OUTTAKE = true sẽ chạy động cơ chiều ngược
            {
                pwm.setPWM(8, 0, 0);
                pwm.setPWM(9, 0, 4096);
            }
    //Chuỗi lệnh để tắt động cơ của intake/outtake, "OFF_IO" viết tắt của "OFF_INTAKE_OUTTAKE"
    if (ps2x.Button(PSB_PAD_LEFT))
    {
        bool INTAKE = false; 
        bool OUTTAKE = false; 
        bool OFF_IO = true;     //Ấn vào nút PAD_LEFT sẽ tắt động cơ của cơ chế để tiết kiệm pin bằng cách cho giá trị boolean OFF_IO = true
    }
    if (OFF_IO == true)        //Nếu OFF_IO = true sẽ tắt động cơ
            {
                pwm.setPWM(8, 0, 0);
                pwm.setPWM(9, 0, 0);
            }
    //Chuỗi lệnh để bật/tắt cơ chế "PUSH"
    if (ps2x.Button(PSB_TRIANGLE))
    {
        bool ON_OFF_P = !ON_OFF_P;      //Ấn vào nút TRIANGLE sẽ tắt/bật cơ chế Push bằng cách đảo ngược lại giá trị của ON_OFF_P
        
    }
    if (ON_OFF_P == true)        //Nếu ON_OFF_P = true sẽ chạy động cơ
        {
            pwm.setPWM(14, 0, 0);
            pwm.setPWM(15, 0, 4096);
        }
    else                         //Nếu ON_OFF_P = false sẽ tắt động cơ để tiết kiệm pin
        {
            pwm.setPWM(14, 0, 0);
            pwm.setPWM(15, 0, 0);
        }
}
