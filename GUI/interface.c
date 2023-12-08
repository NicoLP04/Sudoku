#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <pthread.h>

typedef enum State
{
    FILTER,                           
    DETECT_LINES,                           
    ROTATE,
    NEURAL,
    SOLVE,
    END,
} State;

typedef enum DemoState
{
    FullDemo,
    PartialDemo,
} DemoState;

typedef struct UserInterface
{
    GtkTextView* text_view;
    GtkTextBuffer* buffer;
    DemoState demo_state;
    State state;
    GtkTextView* text_view_image;
    GtkWidget *grid;
    GtkWindow *window;              // Main window
    GtkWidget *displayed_image;           // Drawing area
    GtkButton *nex_button;        // Start button
    GtkButton *all_button;         // Stop button
    GtkCheckButton* training_cb;    // Training check box

    //SECOND TAB
    GtkTextView* text_view_image_secondary;
    GtkWidget *displayed_image_secondary;           // Drawing area
    GtkButton* filter_button;
    GtkButton* rotate_button;
    GtkButton* detect_lines_button;
    GtkButton* solve_button;
    GtkScale* scale;
    double secondary_rotation_angle;

    //modify solver
    GtkGrid* grid_for_modifier;






} UserInterface;

typedef struct Coordinates
{
    size_t i;
    size_t j;
} Coordinates;

#define MAX_FILE_PATH 256


char CurrentSudoku[9][9];

int IMAGE_WIDTH = 600;
int IMAGE_HEIGHT = 600;

char ROTATE_RES_FULL[MAX_FILE_PATH] = "GUI/FullDemoImages/rotated.png";
char ROTATE_RES_PARTIAL[MAX_FILE_PATH] = "GUI/PartialDemoImages/rotated.png";
char GRID_FILE[MAX_FILE_PATH] = "grid_00";

//image_01.jpeg
//image_1.jpeg

//feur.jpeg => image_r.jpeg

char FILTER_RES_FULL[MAX_FILE_PATH] = "GUI/FullDemoImages/";
char FILTER_RES_PARTIAL[MAX_FILE_PATH] = "GUI/PartialDemoImages/";

char DETECT_RES_FULL[MAX_FILE_PATH] = "GUI/FullDemoImages/grid.png";
char DETECT_RES_PARTIAL[MAX_FILE_PATH] = "GUI/PartialDemoImages/grid.png";

gchar ACTUAL_FILE[MAX_FILE_PATH] = "Image/ImageExamples/image_01.jpeg";
gchar ACTUAL_FILE_SECONDARY[MAX_FILE_PATH] = "Image/ImageExamples/Images/image_01.jpeg";
char* ROTATION_ANGLE = "69";



char* get_propper_string(char *str)
{
    size_t i =0;
    int len = strlen(str);
    char* res = malloc(len + 1);
    size_t added = 0;
    size_t counter = 0;
    while(i < len)
    {
        if (str[i] == '\n')
        {
            counter = 0;
        }
        if (counter % 30 == 0 && counter != 0)
        {
            res = realloc(res, len + added + 2);
            res[i + added] = '\n';
            added += 1;
        }
        else
        {
            res[i + added] = str[i];
        }
        //printf("Elements %i : \n Character :%c\n\n", i, res[i + added]);
        counter += 1;
        i++;
    }
    res[i + added] = '\0';
    return res;
}



void set_text_and_center(GtkTextView *text_view, gchar *text, gint width)
{
    //char* propper = get_propper_string(text);


    GtkTextBuffer *buffer = gtk_text_view_get_buffer(text_view);
    gtk_text_buffer_set_text(buffer, text, -1);

    PangoLayout *layout = gtk_widget_create_pango_layout(GTK_WIDGET(text_view), text);

    int text_width, text_height;
    pango_layout_get_pixel_size(layout, &text_width, &text_height);

    if (text_width < width) {
        gint x = (width - text_width) / 2;
        gtk_text_view_set_left_margin(text_view, x);
    } else {
        gtk_text_view_set_left_margin(text_view, 0);
    }
    g_object_unref(layout);
}

// Function to create a GdkPixbuf from a file
GdkPixbuf *create_and_set_pixbuf(const gchar *filename, gpointer user_data)
{
    printf("inside creating pixbuf : %s\n", filename);
    UserInterface * ui = user_data;
    int demo = ui->demo_state == PartialDemo;
    GdkPixbuf *pixbuf;
    GError *error = NULL;
    int x,y,w,h;
    GdkPixbuf *pixbuf_scale;
    pixbuf = gdk_pixbuf_new_from_file(filename, &error);
    if (!pixbuf) {
        fprintf(stderr, "%s\n", error->message);
        g_error_free(error);
    }
    x = 0;
    y = 0;
    h = IMAGE_HEIGHT;
    w = (gdk_pixbuf_get_width(pixbuf) * h) / gdk_pixbuf_get_height(pixbuf);
    pixbuf_scale = gdk_pixbuf_scale_simple(pixbuf, w, h, GDK_INTERP_BILINEAR);
    if (w < IMAGE_WIDTH) 
    {
        x = (IMAGE_WIDTH - w) / 2;
        gtk_layout_move(GTK_LAYOUT(demo ? ui->displayed_image_secondary : ui->displayed_image), demo ? ui->displayed_image_secondary : ui->displayed_image, x, y);
    }
    gtk_image_set_from_pixbuf(GTK_IMAGE(demo ? ui->displayed_image_secondary : ui->displayed_image), pixbuf_scale);
    g_object_unref(pixbuf);
    return pixbuf;
}



// Function to handle the "File Chooser" response
void file_chooser_response(GtkWidget *dialog, gint response_id, gpointer user_data)
{
    UserInterface* ui = user_data;
    int demo = ui->demo_state == PartialDemo;
    if (response_id == GTK_RESPONSE_ACCEPT)
    {
        // Get the selected file from the file chooser
        gchar *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        snprintf(demo ? ACTUAL_FILE_SECONDARY : ACTUAL_FILE, MAX_FILE_PATH, "%s", filename);
        printf("MODIFIED ACTUAL FILE %s\n", demo ? ACTUAL_FILE_SECONDARY : ACTUAL_FILE);
        // Load the image using GdkPixbuf
        create_and_set_pixbuf(filename, user_data);
        set_text_and_center((demo ? ui->text_view_image_secondary : ui->text_view_image), demo ? ACTUAL_FILE_SECONDARY : ACTUAL_FILE, -1);
    }
    // Close the file chooser dialog
    gtk_widget_destroy(dialog);
}

// Function to create and show the file chooser dialog
void choose_image(GtkWidget *button, gpointer user_data)
{
    UserInterface* ui = user_data;
    ui->state = FILTER;
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Open Image",
                                                     GTK_WINDOW(gtk_widget_get_toplevel(button)),
                                                     GTK_FILE_CHOOSER_ACTION_OPEN,
                                                     "Cancel",
                                                     GTK_RESPONSE_CANCEL,
                                                     "Open",
                                                     GTK_RESPONSE_ACCEPT,
                                                     NULL);
    // Show only image files
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_add_pixbuf_formats(filter);
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    // Connect the response signal to the callback function
    g_signal_connect(dialog, "response", G_CALLBACK(file_chooser_response), user_data);

    // Show the file chooser dialog
    gtk_widget_show_all(dialog);
}






gboolean on_configure(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
    g_print("configuring\n");
    UserInterface* ui = user_data;
    gint y_max = gtk_widget_get_allocated_height(GTK_WIDGET(ui->window));
    gint x_max = gtk_widget_get_allocated_width(GTK_WIDGET(ui->window));
    g_print("Xmax : %i\n", x_max);
    g_print("Ymax : %i\n", y_max);
    //gtk_widget_set_size_request(ui->nex_button, 200, 200);
    //gtk_widget_set_size_request(GTK_WIDGET(ui->displayed_image), 800 , 600);
    return FALSE;

    //UserInterface* ui = user_data;
    return FALSE;
}


void handle_rotate(GtkWidget* widget, gpointer data)
{
    UserInterface* ui = data;
    int demo = ui->demo_state == PartialDemo;
    char angle[50];
    if (demo)
    {
        double val = ui->secondary_rotation_angle;
        sprintf(angle, "%lf", val);
    }
    size_t buffer_size = strlen("./Image/Rotation/rotate ") + strlen(demo ? ACTUAL_FILE_SECONDARY : ACTUAL_FILE) + strlen((demo ? angle : ROTATION_ANGLE)) + 1;
    char* buffer = (char *)malloc(buffer_size);
    snprintf(buffer, buffer_size * sizeof(char), "./Image/Rotation/rotate  %s %s", demo ? ACTUAL_FILE_SECONDARY : ACTUAL_FILE, (demo ? angle : ROTATION_ANGLE));
    int status = system(buffer);
    g_free(buffer);
    rename("rotated.png", demo ? "GUI/PartialDemoImages/rotated.png"  : "GUI/FullDemoImages/rotated.png");
    if (WEXITSTATUS(status))
    {
        g_print("External program executed successfully!\n");
        strcpy(demo ? ACTUAL_FILE_SECONDARY : ACTUAL_FILE, demo ? ROTATE_RES_PARTIAL : ROTATE_RES_FULL);
        GdkPixbuf *pixbuf = create_and_set_pixbuf(demo ? ROTATE_RES_PARTIAL : ROTATE_RES_FULL, ui);
        if (pixbuf == NULL)
            g_print("Error creating GdkPixbuf from rotated.png\n");
    }
    else 
        g_print("External program execution failed with status: %d\n", status);
}

void handle_image_to_cell(GtkWidget* widget, gpointer data)
{
    UserInterface* ui = data;
    int demo = ui->demo_state == PartialDemo;

    size_t buffer_size = strlen("./Image/ImageToCells/ImageToCells ") + 
    strlen(demo ? ACTUAL_FILE_SECONDARY : ACTUAL_FILE) + 1;
    char* buffer = (char *)malloc(buffer_size);
    snprintf(buffer, buffer_size * sizeof(char), "./Image/ImageToCells/ImageToCells %s",
    demo ? ACTUAL_FILE_SECONDARY : ACTUAL_FILE);
    printf("Beginning image to cell : %s\n", buffer);
    int status = system(buffer);
    if (WEXITSTATUS(status) != 0)
    {
        return;
    }

    size_t buffer_size2 = strlen("./Image/SolvedImage/Result ") + 
    strlen("grid") + 1;
    char* buffer2 = (char *)malloc(buffer_size2);
    snprintf(buffer2, buffer_size2 * sizeof(char), "./Image/SolvedImage/Result %s",
    "grid");

    status = system(buffer2);

    g_free(buffer);
    rename("gridbefore.png", demo ? "GUI/PartialDemoImages/gridbefore.png"  : "GUI/FullDemoImages/gridbefore.png");
    if (WEXITSTATUS(status))
    {
        set_text_and_center(ui->text_view, "Succesfully detected full image.", 100);
        g_print("External program executed successfully!\n");
        strcpy(demo ? ACTUAL_FILE_SECONDARY : ACTUAL_FILE, demo ? "GUI/PartialDemoImages/gridbefore.png" :
        "GUI/FullDemoImages/gridbefore.png");
        g_print("%s", ACTUAL_FILE);
        GdkPixbuf *pixbuf = create_and_set_pixbuf(demo ? "GUI/PartialDemoImages/gridbefore.png"
        : "GUI/FullDemoImages/gridbefore.png", ui);
        if (pixbuf == NULL)
            g_print("Error creating GdkPixbuf from image.jpeg\n");
    }
    else 
        g_print("External program execution failed with status: %d\n", status);
}

void handle_filter(GtkWidget* widget, gpointer data)
{
    UserInterface* ui = data;
    int demo = ui->demo_state == PartialDemo;

    char num = 0;
    int i;
    for (i = strlen(demo ? ACTUAL_FILE_SECONDARY : ACTUAL_FILE) - 1; i >= 0 &&
    (demo ? ACTUAL_FILE_SECONDARY : ACTUAL_FILE)[i] != '.'; --i)
    ;

    num = (demo ? ACTUAL_FILE_SECONDARY : ACTUAL_FILE)[i - 1];

    char* filename = NULL;

    asprintf(&filename, "image_%c.jpeg", num);
   
    size_t buffer_size = strlen("./Image/ImagePreprocessing/PreProcessing ") + strlen(demo ? ACTUAL_FILE_SECONDARY : ACTUAL_FILE) + 1;
    char* buffer = (char *)malloc(buffer_size);
    snprintf(buffer, buffer_size * sizeof(char), "./Image/ImagePreprocessing/PreProcessing %s", demo ? ACTUAL_FILE_SECONDARY : ACTUAL_FILE);
    int status = system(buffer);
    g_free(buffer);

    char* buffer2 = NULL;
    if (demo)
    {
        asprintf(&buffer2, "GUI/PartialDemoImages/%s", filename);
    }
    else
    {
        asprintf(&buffer2, "GUI/FullDemoImages/%s", filename);
    }
    printf("result : %s\n", buffer2);

    rename((filename), buffer2);
    if (WEXITSTATUS(status) == 0)
    {

        char* buffer3 = NULL;
        asprintf(&buffer3, "%s%s", demo ? FILTER_RES_PARTIAL : FILTER_RES_FULL, buffer2 );

        set_text_and_center(ui->text_view, "Succesfully filtered image. \n\nApplied : \n\n - Grayscale\n\n - Contrast\n\n - Median\n\n - Threshold\n\n - Invert", 100);
        g_print("External program executed successfully!\n");
        strcpy(demo ? ACTUAL_FILE_SECONDARY : ACTUAL_FILE, buffer2);
        g_print("%s", ACTUAL_FILE);
        GdkPixbuf *pixbuf = create_and_set_pixbuf(buffer2, ui);
        if (pixbuf == NULL)
            g_print("Error creating GdkPixbuf from image.jpeg\n");
    }
    else 
        g_print("External program execution failed with status: %d\n", status);
}

void handle_grid_detection(GtkWidget* widget, gpointer data)
{
    UserInterface* ui = data;
    int demo = ui->demo_state == PartialDemo;
    size_t buffer_size = strlen("./Image/GridDetection/GridDetection ") + 
    strlen(demo ? ACTUAL_FILE_SECONDARY : ACTUAL_FILE) + 1;
    char* buffer = (char *)malloc(buffer_size);
    snprintf(buffer, buffer_size * sizeof(char), "./Image/GridDetection/GridDetection %s",
    demo ? ACTUAL_FILE_SECONDARY : ACTUAL_FILE);
    printf("Beginning detection on : %s\n", buffer);
    int status = system(buffer);
    g_free(buffer);
    rename("grid.png", demo ? "GUI/PartialDemoImages/grid.png"  : "GUI/FullDemoImages/grid.png");
    if (WEXITSTATUS(status) == 0)
    {
        set_text_and_center(ui->text_view, "Succesfully filtered image. \n\nApplied : \n\n - Grayscale\n\n - Contrast\n\n - Median\n\n - Threshold\n\n - Invert", 100);
        g_print("External program executed successfully!\n");
        strcpy(demo ? ACTUAL_FILE_SECONDARY : ACTUAL_FILE, demo ? DETECT_RES_PARTIAL : DETECT_RES_FULL);
        g_print("%s", ACTUAL_FILE);
        GdkPixbuf *pixbuf = create_and_set_pixbuf(demo ? DETECT_RES_PARTIAL : DETECT_RES_FULL, ui);
        if (pixbuf == NULL)
            g_print("Error creating GdkPixbuf from image.jpeg\n");
    }
    else 
        g_print("External program execution failed with status: %d\n", status);
}


void handle_solve(GtkWidget* widget, gpointer data)
{
    
}

void load_all_images()
{
    
}

void next_step_button(GtkWidget *widget, gpointer data)
{

    UserInterface* ui = data;
    switch (ui->state)
    {
        case NEURAL:
            ui->state = SOLVE;
            handle_image_to_cell(widget, data); break;
        case FILTER:
            ui->state = DETECT_LINES;
            handle_filter(widget, data); break;
        case DETECT_LINES:
            ui->state = NEURAL;
            handle_grid_detection(widget, data); break;
        case ROTATE:
            ui->state = NEURAL;
            handle_rotate(widget, data); break;
        case SOLVE:
            ui->state = END;
            handle_solve(widget, data); break;
        case END:
            break;
    }
}

void open_explorer(gpointer data)
{
    const gchar *current_directory = g_get_current_dir();
    gchar *command = g_strdup_printf("xdg-open \"%s\"", current_directory);
    GError *error = NULL;
    if (!g_spawn_command_line_async(command, &error)) {
        g_print("Error launching file explorer: %s\n", error->message);
        g_error_free(error);
    }
    g_free(command);
}


// Callback function for the button click event
void next_step(GtkWidget *widget, gpointer data) 
{
    g_print("Next step!\n");
}

// Callback function for the button click event
void all_step(GtkWidget *widget, gpointer data) 
{
    g_print("All steps!\n");
}


void crop_button_handler(GtkWidget* widget, gpointer data)
{
    handle_image_to_cell(widget, data);
}


void rotate_button_handler(GtkWidget* widget, gpointer data)
{
    handle_rotate(widget, data);
}

void filter_button_handler(GtkWidget* widget, gpointer data)
{
     handle_filter(widget, data);
}

void detect_button_handler(GtkWidget* widget, gpointer data)
{
     handle_grid_detection(widget, data);
}

void solve_button_handler(GtkWidget* widget, gpointer data)
{
    handle_solve(widget, data);
}


void setup_solver(gpointer data)
{
    UserInterface *ui = data;
    char *filename = GRID_FILE;
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("Error: could not open file %s\n", filename);
        return;
    }  

    for (int i = 0; i < 9; i++) 
        {   
            for (int j = 0; j < 9; j++)
            {
                    CurrentSudoku[i][j] = ' ';
            }
        }

        for (int i = 0; i < 9; i++) 
        {   
            for (int j = 0; j < 9; j++)
            {
                char ch = fgetc(fp);
                while (ch == ' ' || ch == '\n' || ch == '\r') {
                    ch = fgetc(fp);
                }
                if (ch >= '0' && ch <= '9')
                {
                    
                    char* add = malloc(sizeof(char));
                    sprintf(add, "%c",ch);
                    GtkWidget *child = gtk_grid_get_child_at(ui->grid_for_modifier, j, i);
                    gtk_entry_set_text(GTK_ENTRY(child) , add);
                    if (ch == '.')
                            CurrentSudoku[i][j] = ' ';
                    else
                    {
                            CurrentSudoku[i][j] = ch;
                    }
                    printf("setting current : %c\n", CurrentSudoku[i][j]);
                    free(add);
                } 
            }
    }

    
    
   
}








void on_tab_switched(GtkNotebook *notebook, GtkWidget *page, guint page_num, gpointer user_data)
{

    g_print("Switched to tab %u\n", page_num);
    UserInterface* ui = user_data;
    switch (page_num)
    {
        case 0:
            ui->demo_state = FullDemo; break;
        case 1:
            ui->demo_state = PartialDemo; break;
        default:
            ui->demo_state = FullDemo; break;
    }
    // Add your code here to perform actions when a tab is switched
}

void on_scale_value_changed(GtkRange *range, gpointer user_data)
{
    UserInterface* ui = user_data;
    gdouble value = gtk_range_get_value(range);
    ui->secondary_rotation_angle = value;
    g_print("Scale value: %.2f\n", value);
}


void on_css_parsing_error(GtkCssProvider* provider, GError* error, gpointer user_data) {
    g_printerr("CSS Parsing Error: %s\n", error->message);
}



void apply_dynamic_style(GtkWidget *widget, const gchar *style_name) {
    // Set a name for the widget
    gtk_widget_set_name(widget, style_name);

    // Get the style context and force a refresh
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    gtk_style_context_save(context);
    gtk_style_context_add_class(context, style_name);
    gtk_style_context_restore(context);
}


size_t MATRIX_SIZE = 9;

void writeMatrixToFile(const char* filename) {
    FILE *file = fopen(filename, "w");

    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++)
        {
            char value = CurrentSudoku[i][j];
            if (value == ' ')
            {
                fprintf(file, ".");
            } else {
                fprintf(file, "%c", value);
            }

            if ((j + 1)% 3 == 0) {
                fprintf(file, " ");
            }
        }
        if ((i + 1 )% 3 == 0)
            fprintf(file, "%c",'\n');

        fprintf(file, "\n");
    }
    fclose(file);
}

void entry_changed(GtkEditable *editable, gpointer data)
{

    size_t  i = GPOINTER_TO_INT(data);
        size_t j = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(editable), "param2"));
    const gchar *text = gtk_entry_get_text(GTK_ENTRY(editable));
    CurrentSudoku[i][j] = ' ';

    //printf("recieved and adding on (%zu, %zu) : %c\n", text[0], i, j);
    CurrentSudoku[i][j] = text[0];
    writeMatrixToFile("res");

}



int main(int argc, char *argv[]) {


    GtkWindow *window;
    GtkWidget *image;
    GtkWidget *nex_button;
    GtkButton *file_explorer_button;
    GtkButton *all_button;
    GtkWidget *text_view;
    GtkTextBuffer *text_buffer;
    GtkWidget* text_view_image;
    gchar *filename;

    GtkWidget* text_view_secondary;


    GtkGrid* grid_for_modifier;
    GtkButton* open_button_secondary;
    GtkButton* filter_button;
    GtkButton* rotate_button;
    GtkButton* detect_lines_button;
    GtkButton* solve_button;
    GtkScale* scale;
    GtkWidget* secondary_image;
    GtkButton* crop_button;


    if (argc < 2)
    {
        printf("missing image\n");
        filename = ACTUAL_FILE;
    }
    else
    {
        filename = argv[1];
        strcpy(ACTUAL_FILE, filename);
        printf("Beginning : %s\n", ACTUAL_FILE);
    }


    gtk_init(&argc, &argv);

    GtkBuilder *builder;

    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "GUI/interface.glade", NULL);


    filter_button = GTK_BUTTON(gtk_builder_get_object(builder, "filter_button"));
    detect_lines_button = GTK_BUTTON(gtk_builder_get_object(builder, "detect_lines_button"));
    solve_button = GTK_BUTTON(gtk_builder_get_object(builder, "solve_button"));
    rotate_button = GTK_BUTTON(gtk_builder_get_object(builder, "rotate_button"));
    scale = GTK_SCALE(gtk_builder_get_object(builder, "rotate_scale"));
    



    gtk_range_set_range(GTK_RANGE(scale), -360, 360);
    gtk_range_set_increments(GTK_RANGE(scale), 1, 1);

    g_object_set(scale, "width-request", 200, NULL); 

  

    GtkWidget* notebook = GTK_WIDGET(gtk_builder_get_object(builder, "notebook1"));
    window = GTK_WINDOW(gtk_builder_get_object(builder, "main_window"));
    //grid = GTK_WIDGET(gtk_builder_get_object(builder, "grid_full"));
    //grid_partial = GTK_WIDGET(gtk_builder_get_object(builder, "grid_partial"));
    nex_button = (gtk_builder_get_object(builder, "next_step"));
    all_button = GTK_BUTTON(gtk_builder_get_object(builder, "all_step"));
    file_explorer_button = GTK_BUTTON(gtk_builder_get_object(builder, "open_explorer"));
    crop_button = GTK_BUTTON(gtk_builder_get_object(builder, "crop_button"));


    open_button_secondary = GTK_BUTTON(gtk_builder_get_object(builder, "open_explorer_second"));
    secondary_image = GTK_WIDGET(gtk_builder_get_object(builder, "image_displayed1"));
    image = GTK_WIDGET(gtk_builder_get_object(builder, "image_displayed")); //GTK_IMAGE(gtk_builder_get_object(builder, "image_displayed"));
    text_view = GTK_WIDGET(gtk_builder_get_object(builder, "text_view"));
    text_view_image = GTK_WIDGET(gtk_builder_get_object(builder, "text_view_image"));
    text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    grid_for_modifier = GTK_GRID(gtk_builder_get_object(builder, "grid_modifier"));


    text_view_secondary = GTK_WIDGET(gtk_builder_get_object(builder, "text_view_image2"));
    gtk_window_set_title(GTK_WINDOW(window), "OCR Sudoku Solver");
    set_text_and_center(GTK_TEXT_VIEW(text_view), "Welcome to the OCR sudoku solver.\n \n- Select your sudoku image from your files\n \n- Use the next step button to proceed to the next step of the resolution of your sudoku\n \n- Enjoy !\n \nIn case no images are selected, you can use the provided example", 100);
    gtk_widget_set_size_request(GTK_WIDGET(window), 800, 800);
    gtk_widget_set_size_request(GTK_WIDGET(file_explorer_button), 100 , 100);

    set_text_and_center(GTK_TEXT_VIEW(text_view_image), ACTUAL_FILE, -1);
    UserInterface ui = {
                .grid_for_modifier = grid_for_modifier,
                .text_view_image_secondary = GTK_TEXT_VIEW(text_view_secondary),
                .displayed_image_secondary = secondary_image,
                .text_view_image = GTK_TEXT_VIEW(text_view_image),
                .demo_state = FullDemo,
                .text_view = GTK_TEXT_VIEW(text_view),
                .buffer = text_buffer,
                .state = FILTER,
                .grid = NULL,
                .window = window,
                .displayed_image = image,
                .all_button = (all_button),
                .nex_button = GTK_BUTTON(nex_button),
                .training_cb = NULL,
        };

   

    for (size_t i = 0; i < 9; i++)
    {
        for (size_t j = 0; j < 9; j++)
        {
            GtkWidget *text_entry = gtk_entry_new();
            gtk_entry_set_alignment(GTK_ENTRY(text_entry), 0.5); // Center the text
            gtk_widget_set_size_request(text_entry, 25, 5); // Set the size of the text entry
            gtk_grid_attach(grid_for_modifier, text_entry, j,i , 1, 1);
            g_signal_connect(G_OBJECT(text_entry), "changed", G_CALLBACK(entry_changed), GINT_TO_POINTER(i));
            g_object_set_data(G_OBJECT(text_entry), "param2", GINT_TO_POINTER(j));
        }
    }

    gtk_grid_set_row_spacing(grid_for_modifier, 0);
    gtk_grid_set_column_spacing(grid_for_modifier, 0);
    setup_solver(&ui);

    gtk_widget_set_halign(grid_for_modifier, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(grid_for_modifier, GTK_ALIGN_CENTER);

    g_signal_connect(G_OBJECT(scale), "value-changed", G_CALLBACK(on_scale_value_changed), &ui);
    g_signal_connect(G_OBJECT(notebook), "switch-page", G_CALLBACK(on_tab_switched), &ui);
    g_signal_connect(filter_button, "clicked", G_CALLBACK(filter_button_handler), &ui);
    g_signal_connect(detect_lines_button, "clicked", G_CALLBACK(handle_grid_detection), &ui);
    g_signal_connect(rotate_button, "clicked", G_CALLBACK(rotate_button_handler), &ui);
    g_signal_connect(solve_button, "clicked", G_CALLBACK(solve_button), &ui);
    g_signal_connect(crop_button, "clicked", G_CALLBACK(solve_button), &ui);



    g_signal_connect(open_button_secondary, "clicked", G_CALLBACK(handle_image_to_cell), &ui);


    gtk_widget_set_name((all_button), "button");
    gtk_widget_set_name((nex_button), "button");
    gtk_widget_set_name((text_view), "textview");

    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(provider, "style.css", NULL);


    GtkStyleContext *context = gtk_widget_get_style_context((all_button));
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    context = gtk_widget_get_style_context((nex_button));
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    context = gtk_widget_get_style_context(GTK_WIDGET(text_view));
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);




    // Connect the CSS provider's parsing-error signal to handle any errors
        //apply_dynamic_style(nex_button, "my-custom-class");

    // Create a CSS style provider
   /* GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider, "window {color: yellow; background: #0000aa;}", -1, NULL);
    GtkStyleContext *context = gtk_widget_get_style_context(window);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);


    GtkCssProvider *provider2 = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider2, "text_view {color: yellow; background: #0000aa;}", -1, NULL);
    GtkStyleContext * context2 = gtk_widget_get_style_context(text_view);
    gtk_style_context_add_provider(context2, GTK_STYLE_PROVIDER(provider2), GTK_STYLE_PROVIDER_PRIORITY_USER);*/

    /*GdkRGBA color;
    gdk_rgba_parse(&color, "rgb(255, 0, 0)");  // Set the color to red

    // Set the background color of the window
    gtk_widget_override_color(GTK_WIDGET(window), GTK_STATE_FLAG_ACTIVE, &color);*/

    GdkPixbuf *pixbuf = create_and_set_pixbuf(filename, &ui);
    gtk_image_set_from_pixbuf(GTK_IMAGE(image), pixbuf);
    g_object_unref(pixbuf);

    ui.demo_state = PartialDemo;
    GdkPixbuf *pixbuf2 = create_and_set_pixbuf(filename, &ui);
    gtk_image_set_from_pixbuf(GTK_IMAGE(secondary_image), pixbuf2);
    g_object_unref(pixbuf2);
    ui.demo_state = FullDemo;
    const GdkPixbuf *pb = gtk_image_get_pixbuf(GTK_IMAGE(image));

    IMAGE_WIDTH = gdk_pixbuf_get_width(pb);
    IMAGE_HEIGHT = gdk_pixbuf_get_height(pb);

    g_signal_connect(file_explorer_button, "clicked", G_CALLBACK(choose_image), &ui);
    g_signal_connect(nex_button, "clicked", G_CALLBACK(next_step_button), &ui);
    g_signal_connect(window, "configure-event", G_CALLBACK(on_configure), &ui);
    // Show all widgets

    /*GdkRGBA color;
    gdk_rgba_parse(&color, "rgb(0, 0, 255)");  // Set the color to blue

    // Create a CSS style provider
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider, "next_step { background-color: rgba(0, 0, 255, 1); }", -1, NULL);

    // Apply the CSS style provider to the button
    GtkStyleContext *context = gtk_widget_get_style_context(nex_button);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);*/

    //gtk_widget_set_child_visible(grid, ui.demo_state == FullDemo);
    //gtk_widget_set_child_visible(grid_partial, ui.demo_state == PartialDemo);
    gtk_widget_show_all(GTK_WIDGET(window));
        g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    // Start the GTK main loop
    gtk_main();

    return 0;
}