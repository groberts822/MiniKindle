# MiniKindle
A mini e-reader/kindle!
---
Final Design:

<img width="603" height="284" alt="image" src="https://github.com/user-attachments/assets/7df36c10-7c07-49d1-8cfc-ec645da95ffd" />

<img width="861" height="511" alt="image" src="https://github.com/user-attachments/assets/5cbd8196-4ed4-4864-83cb-c9ea94dec21b" />

CAD Models:


<img width="646" height="646" alt="image" src="https://github.com/user-attachments/assets/330b2cda-53c8-400d-b4d8-f8adcff01492" />

Note for the CAD design. There are holes to connect the wires from the switch and TP4056 (the pos and neg) to the pins of the ESP32C3, its just hard to get a good picture of those. Also, the battery setup will be soldered to the 5V and GND pins of the microcontroller as the battery pads are covered by the case, and its simply easier to access the 5V pin, which works just fine as it can take the 3.7V down to the 3.3V fine.
Another note, the case will be assembled by using super glue to secure all 3D printed parts to one another. The PCB fits right into place and (should) fit securely enough to keep it in place without wiggling. 

Assembly:

<img width="770" height="775" alt="image" src="https://github.com/user-attachments/assets/957d3493-49e9-4e25-a22a-b817fb5aa112" />

Just a note, the eink display will be mounted to the board through the pins for the white connector. It may seem overly complicated, but i've done it for projects before and it's easier than it seems. It will simply be soldered from the pins there to the soldering holes using wires, as there isnt really an easier, compact way to connect it, and I just prefer this method.

Schematic:

<img width="635" height="477" alt="image" src="https://github.com/user-attachments/assets/b338aa8b-8704-4c72-ae61-d9d0a0746847" />

PCB: 

<img width="1085" height="456" alt="image" src="https://github.com/user-attachments/assets/87223d3f-5166-4074-81a3-23f5720337d3" />

3D of PCB with all Parts

<img width="625" height="398" alt="image" src="https://github.com/user-attachments/assets/bac792b3-08c5-4e81-aad0-eba02a4d6e0f" />

<img width="718" height="400" alt="image" src="https://github.com/user-attachments/assets/138e2c49-4a3a-46f3-8edc-38ce2907a07b" />

Bill of Materials (also in .csv format)

| Item | Order Qty | Actual Amount Needed | Link |
|------|-----------|----------------------|------|
| Seeed Studio XIAO ESP32C3 | 1 | 1 | [Amazon](https://www.amazon.com/Seeed-Studio-XIAO-ESP32C3-Microcontroller/dp/B0B94JZ2YF/ref=sr_1_1_sspa?crid=25832RCF5SI0L&dib=eyJ2IjoiMSJ9.TUcu_JgOx4dykbtNXTKjde7ZwMAO9MNOl8WPl3lLjWA6_lh0zWqhFu8ssi_SoTl5KFVqyTZ9tAgb9PqizaJ32ZJKAd0hEp3TdaoLNJGKB1XUTIDDk0DZsTizZOOhf0xI46QUTnPtJs7RfaKayrCNN0LIqaFG8_6GSIPwPxTEhi2GGw4rubu_2GTfcJsvyHfB2icBKRNOjMpKD_wChiwkaBhNl56pvX0_OdoNT6kPr24N9x2JQELeAxxMpyM7SNXkSLPMREKSRGDVO3RT_nsXaD5XVOyTtIC9oOpkqvegOE4.Z5N7m13gSUHiz2zxnUHAEa4MM69LdYGM9WdJV3czJE8&dib_tag=se&keywords=esp32c3&qid=1773078950&s=electronics&sprefix=esp32c3%2Celectronics%2C210&sr=1-1-spons&sp_csd=d2lkZ2V0TmFtZT1zcF9hdGY&th=1) |
| Waveshare 2.13in E-Ink Display HAT | 1 | 1 | [Amazon](https://www.amazon.com/waveshare-2-13inch-HAT-Compatible-Resolution/dp/B071S8HT76/) |
| ANMBEST Micro SD Card Adapter | 1 pack (10pcs) | 1 | [Amazon](https://www.amazon.com/ANMBEST-Adapter-Interface-Conversion-Arduino/dp/B08CMLG4D6/) |
| DAOKI 6mm Tactile Push Buttons | 1 pack (100pcs) | 2 | [Amazon](https://www.amazon.com/DAOKI-Miniature-Momentary-Tactile-Quality/dp/B01CGMP9GY/) |
| MakerFocus 3.7V 1100mAh LiPo Battery | 1 pack (4pcs) | 1 | [Amazon](https://www.amazon.com/1100mAh-Rechargable-Protection-Insulated-Development/dp/B0867KDMY7/) |
| ALAMSCN TP4056 Type-C Charger | 1 pack (12pcs) | 1 | [Amazon](https://www.amazon.com/ALAMSCN-Lithium-Charging-Protection-Functions/dp/B08F7QSLN7/) |
| Mini Micro Slide Switch | 1 pack (20pcs) | 1 | [Amazon](https://www.amazon.com/Mini-Micro-Slide-Switch-Breadboard/dp/B0DN69L9SG/) |

------------------------------------------------------
I wanted a reading device which didn't feel like a smartphone. Most modern screens are meant to grab our attention with the backlight and notifications. This project is about the opposite, being a distraction-free and almost "offline" experience. This project uses an E-ink display and the text looks like actuasl paper and doesn't strain the eyes. Plus since the E-ink only uses power when the page changes initially, I can leave a book open for long periods of time without the battery dying. 

The brain of the project is a XIAO ESP32C3. When you flip the physical slide switch the system boots and reads the book files (.txt files) from the MicroSD card. There are 2 tactile buttons which allow you to essentially turn the page. It uses a 2.13-in electronic paper display (EPD) and is crisp in direct sunlight and stays on/displays text even after power is cut. A TP4056 module manages the 1100mAh LiPo battery, which you can charge via USB-C, and the slide switch makes sure the battery doesnt drain when the reader is not in use. 
