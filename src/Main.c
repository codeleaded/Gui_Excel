#if defined(__linux__) && !defined(_WIN32)
    #include "/home/codeleaded/System/Static/Library/WindowEngine1.0.h"
    #include "/home/codeleaded/System/Static/Library/ExcelRenderer.h"
#elif defined(_WIN32) || defined(_WIN64)
    #include "/home/codeleaded/System/Static/Library/WindowEngine1.0.h"
    #include "/home/codeleaded/System/Static/Library/ExcelRenderer.h"
#elif defined(__APPLE__)
    #error "Apple not supported!"
#else
    #error "Platform not supported!"
#endif



#define OFFSET_X    1.0f
#define OFFSET_Y    300.0f
#define PADDING_Y   10.0f

#define CELL_DX     400.0f
#define CELL_DY     200.0f

Excel excel;
int cellx;
int celly;
TextBox edited;
float scrollx;
float scrolly;

void Setup(AlxWindow* w){
	ResizeAlxFont(25,25);

    edited = TextBox_New(Input_New(10,INPUT_MAXLENGTH),(Rect){ 0.0f,0.0f,1000.0f,OFFSET_Y - PADDING_Y },ALXFONT_PATHS_HIGH,16,32,BLACK);
    TextBox_SetSyntax(&edited, "/home/codeleaded/System/SyntaxFiles/VBLike_Syntax.alxon");

    excel = Excel_Load("./bin","./data/SpreadSheet0.vblss");
    // excel = Excel_New("./bin",10,10);
    // Excel_Print(&excel);
    // Excel_Print_T(&excel);
    // Excel_Print_O(&excel);
}

void Update(AlxWindow* w){
    char isEnabled = edited.In.Enabled;
    TextBox_Update(&edited,w->Strokes,GetMouse());
	
    if(!isEnabled){
        cellx = -1;
        celly = -1;

        if(Stroke(ALX_MOUSE_L).PRESSED){
            const unsigned int ix = (unsigned int)floorf((float)(w->MouseX - OFFSET_X) / (float)CELL_DX);
            const unsigned int iy = (unsigned int)floorf((float)(w->MouseY - OFFSET_Y) / (float)CELL_DY);

            ExcelCell* ec = Excel_Get(&excel,ix,iy);
            if(ec){
                cellx = ix;
                celly = iy;
                Input_SetText(&edited.In,ec->data);
                edited.In.Enabled = 1;
                edited.In.Curser = 0;
                edited.In.CurserEnd = 0;
            }
        }
        
        if(Stroke(ALX_KEY_UP).DOWN)         scrolly -= 500.0f * w->ElapsedTime;
        else if(Stroke(ALX_KEY_DOWN).DOWN)  scrolly += 500.0f * w->ElapsedTime;
        
        if(Stroke(ALX_KEY_LEFT).DOWN)       scrollx -= 500.0f * w->ElapsedTime;
        else if(Stroke(ALX_KEY_RIGHT).DOWN) scrollx += 500.0f * w->ElapsedTime;

        if(scrolly < 0.0f) scrolly = 0.0f;
        if(scrollx < 0.0f) scrollx = 0.0f;
    }else{
        if(Stroke(ALX_MOUSE_L).PRESSED){
            const unsigned int ix = (unsigned int)floorf((float)(w->MouseX - OFFSET_X) / (float)CELL_DX);
            const unsigned int iy = (unsigned int)floorf((float)(w->MouseY - OFFSET_Y) / (float)CELL_DY);

            if(ix == cellx && iy == celly){
                ExcelCell* ec = Excel_Get(&excel,cellx,celly);
                if(ec){
                    CStr cstr = String_CStr(&edited.In.Buffer);
                    Excel_Set(&excel,cellx,celly,cstr);
                    CStr_Free(&cstr);

                    edited.In.Enabled = 0;
                }
            }
        }
    }

	Clear(BLACK);

    for(int i = 0;i<excel.county;i++){
        for(int j = 0;j<excel.countx;j++){
            CStr content = Excel_GetCStr(&excel,j,i);
            
            const float px = OFFSET_X - scrollx + j * CELL_DX;
            const float py = OFFSET_Y - scrolly + i * CELL_DY;

            if(content){
                CStr_RenderAlxFont(
                    WINDOW_STD_ARGS,
                    &w->AlxFont,
                    content,
                    px,
                    py,
                    WHITE
                );

                CStr_Free(&content);
            }

            Rect_RenderXXWire(WINDOW_STD_ARGS,px,py,CELL_DX,CELL_DY,WHITE,1.0f);
        }
    }

    if(cellx >= 0 && celly >= 0){
        const float px = OFFSET_X - scrollx + cellx * CELL_DX;
        const float py = OFFSET_Y - scrolly + celly * CELL_DY;
        Rect_RenderXXWire(WINDOW_STD_ARGS,px,py,CELL_DX,CELL_DY,GREEN,1.0f);
    }

	TextBox_Render(WINDOW_STD_ARGS,&edited);
}

void Delete(AlxWindow* w){
    Excel_Print(&excel);
    Excel_Print_T(&excel);
    Excel_Print_O(&excel);
    Excel_Save(&excel,"./data/SpreadSheet0.vblss");
    
    Excel_Free(&excel);
}

int main(){
    if(Create("Gui Excel",2500,1200,1,1,Setup,Update,Delete))
        Start();
    return 0;
}