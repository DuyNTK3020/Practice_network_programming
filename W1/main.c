#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME "user.txt"
#define MAX_USERNAME_LENGTH 100
#define MAX_PASSWORD_LENGTH 100

typedef struct User
{
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    int status; // 1 = active, 0 = blocked
    int attempts;
    struct User *next;
} User;

typedef struct LoggedInUser
{
    char username[MAX_USERNAME_LENGTH];
    struct LoggedInUser *next;
} LoggedInUser;

User *head = NULL;
LoggedInUser *loggedInHead = NULL;

void loadUsersFromFile()
{
    FILE *file = fopen(FILENAME, "r");
    if (file == NULL)
    {
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file))
    {
        User *newUser = (User *)malloc(sizeof(User));
        if (newUser == NULL)
        {
            printf("Memory allocation failed.\n");
            fclose(file);
            exit(1);
        }

        char *token = strtok(line, ":");
        strcpy(newUser->username, token);

        token = strtok(NULL, ":");
        strcpy(newUser->password, token);

        token = strtok(NULL, ":");
        newUser->status = atoi(token);

        newUser->attempts = 0;

        newUser->next = head;
        head = newUser;
    }

    fclose(file);
}

User *findUser(const char *username)
{
    User *current = head;
    while (current != NULL)
    {
        if (strcmp(current->username, username) == 0)
        {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void saveUsersToFile()
{
    FILE *file = fopen(FILENAME, "w");
    if (file == NULL)
    {
        printf("Unable to open file for writing.\n");
        exit(1);
    }

    User *current = head;
    while (current != NULL)
    {
        fprintf(file, "%s:%s:%d\n", current->username, current->password, current->status);
        current = current->next;
    }

    fclose(file);
}

void displayMenu()
{
    printf("\nUSER MANAGEMENT PROGRAM\n");
    printf("-----------------------------------\n");
    printf("1. Register\n");
    printf("2. Sign in\n");
    printf("3. Search\n");
    printf("4. Sign out\n");
}

void registerUser()
{
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    User *existingUser;

    printf("Enter username: ");
    scanf("%s", username);

    existingUser = findUser(username);
    if (existingUser != NULL)
    {
        printf("Username already exists.\n");
        return;
    }

    printf("Enter password: ");
    scanf("%s", password);

    User *newUser = (User *)malloc(sizeof(User));
    if (newUser == NULL)
    {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    strcpy(newUser->username, username);
    strcpy(newUser->password, password);
    newUser->status = 1;
    newUser->attempts = 0;
    newUser->next = head;
    head = newUser;

    saveUsersToFile();
    printf("User registered successfully.\n");
}

void addLoggedInUser(const char *username)
{
    LoggedInUser *newLoggedInUser = (LoggedInUser *)malloc(sizeof(LoggedInUser));
    if (newLoggedInUser == NULL)
    {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    strcpy(newLoggedInUser->username, username);
    newLoggedInUser->next = loggedInHead;
    loggedInHead = newLoggedInUser;
}

void removeLoggedInUser(const char *username)
{
    LoggedInUser *current = loggedInHead;
    LoggedInUser *prev = NULL;

    while (current != NULL)
    {
        if (strcmp(current->username, username) == 0)
        {
            if (prev == NULL)
            {
                loggedInHead = current->next;
            }
            else
            {
                prev->next = current->next;
            }
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}

int isUserLoggedIn(const char *username)
{
    LoggedInUser *current = loggedInHead;
    while (current != NULL)
    {
        if (strcmp(current->username, username) == 0)
        {
            return 1; // User is already logged in
        }
        current = current->next;
    }
    return 0; // User is not logged in
}

void signIn()
{
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    User *user;

    printf("Enter username: ");
    scanf("%s", username);

    if (isUserLoggedIn(username))
    {
        printf("User is already signed in.\n");
        return;
    }

    user = findUser(username);
    if (user == NULL)
    {
        printf("Username does not exist.\n");
        return;
    }

    if (user->status == 0)
    {
        printf("Account is blocked.\n");
        return;
    }

    while (user->attempts < 3)
    {
        printf("Enter password: ");
        scanf("%s", password);

        if (strcmp(user->password, password) == 0)
        {
            printf("Sign in successful.\n");
            user->attempts = 0;
            addLoggedInUser(username);
            return;
        }
        else
        {
            printf("Incorrect password.\n");
            user->attempts++;
        }
    }

    user->status = 0; // Block the account
    saveUsersToFile();
    printf("Account has been blocked due to too many incorrect attempts.\n");
}

void searchUser()
{
    char username[MAX_USERNAME_LENGTH];
    User *user;

    printf("Enter username: ");
    scanf("%s", username);

    user = findUser(username);
    if (user == NULL)
    {
        printf("User not found.\n");
    }
    else
    {
        printf("Username: %s\n", user->username);
        printf("Status: %s\n", user->status == 1 ? "active" : "blocked");
    }
}

void signOut()
{
    char username[MAX_USERNAME_LENGTH];
    User *user;

    printf("Enter username: ");
    scanf("%s", username);

    user = findUser(username);
    if (user == NULL)
    {
        printf("User not found\n");
    }
    else if (!isUserLoggedIn(user->username))
    {
        printf("User not signed in.\n");
    }
    else
    {
        removeLoggedInUser(username);
        printf("Sign out successful.\n");
    }
}

void freeUserList()
{
    User *current = head;
    User *next;

    while (current != NULL)
    {
        next = current->next;
        free(current);
        current = next;
    }
}

void freeLoggedInUserList()
{
    LoggedInUser *current = loggedInHead;
    LoggedInUser *next;

    while (current != NULL)
    {
        next = current->next;
        free(current);
        current = next;
    }
}

int main()
{
    int choice;
    loadUsersFromFile();
    while (1)
    {
        displayMenu();
        printf("Your choice (1-4, other to quit): ");
        if (scanf("%d", &choice) != 1)
        {
            break;
        }
        switch (choice)
        {
        case 1:
            registerUser();
            break;
        case 2:
            signIn();
            break;
        case 3:
            searchUser();
            break;
        case 4:
            signOut();
            break;
        default:
            freeUserList();
            freeLoggedInUserList();
            exit(0);
        }
    }

    freeUserList();
    freeLoggedInUserList();
    return 0;
}