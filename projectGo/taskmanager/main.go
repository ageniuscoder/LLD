package main

import (
	"fmt"
	"sync"
	"taskmanager/app"
	enum "taskmanager/enums"
	"taskmanager/notification"
	"taskmanager/task"
	"taskmanager/user"
)

func main() {
	fmt.Println("╔══════════════════════════════════════════╗")
	fmt.Println("║   Task Manager — Concurrent Safety Demo  ║")
	fmt.Println("╚══════════════════════════════════════════╝")

	// ── 1. Wire dependencies ────────────────────────────────────────────────
	// Swap NewEmailNotification() here — TaskManager needs zero changes.
	notifier := notification.NewInAppNotification()
	um := user.NewUserManager()
	tm := task.NewTaskManager(notifier)
	taskApp := app.NewTaskApp(um, tm)

	// ── 2. Register users ───────────────────────────────────────────────────
	fmt.Println("\n--- Registering Users ---")
	admin := taskApp.RegisterUser("Priya Sharma", "priya@company.com")
	dev1 := taskApp.RegisterUser("Arjun Mehta", "arjun@company.com")
	dev2 := taskApp.RegisterUser("Sneha Kapoor", "sneha@company.com")
	dev3 := taskApp.RegisterUser("Rohan Verma", "rohan@company.com")

	// ── 3. Admin creates tasks ──────────────────────────────────────────────
	fmt.Println("\n--- Creating Tasks ---")
	tid1 := taskApp.CreateTask(admin.GetId(), "Design DB Schema", "Payments module schema", 3, enum.HIGH)
	tid2 := taskApp.CreateTask(admin.GetId(), "Build REST API", "CRUD for /payments", 7, enum.HIGH)
	tid3 := taskApp.CreateTask(admin.GetId(), "Write Unit Tests", "Table-driven tests", 10, enum.MEDIUM)
	tid4 := taskApp.CreateTask(admin.GetId(), "Update API Docs", "Swagger spec", 14, enum.LOW)
	tid5 := taskApp.CreateTask(admin.GetId(), "Fix Login Bug", "JWT not refreshing", 1, enum.HIGH)

	// ── 4. Assign tasks ─────────────────────────────────────────────────────
	fmt.Println("\n--- Assigning Tasks ---")
	taskApp.AssignTask(admin.GetId(), dev1.GetId(), tid1)
	taskApp.AssignTask(admin.GetId(), dev1.GetId(), tid2)
	taskApp.AssignTask(admin.GetId(), dev2.GetId(), tid3)
	taskApp.AssignTask(admin.GetId(), dev2.GetId(), tid4)
	taskApp.AssignTask(admin.GetId(), dev3.GetId(), tid5)

	// ── 5. Double-assign rejection ──────────────────────────────────────────
	fmt.Println("\n--- Double-assign (expected rejection) ---")
	taskApp.AssignTask(admin.GetId(), dev2.GetId(), tid1)

	// ── 6. Concurrent task completion ───────────────────────────────────────
	// This is the key test: three developers complete tasks at the same time.
	// Without the mutex in manager.go, this would cause a data race.
	// Run with: go run -race ./...  to verify.
	fmt.Println("\n--- Concurrent Task Completion ---")
	var wg sync.WaitGroup

	wg.Add(1)
	go func() {
		defer wg.Done()
		taskApp.CompleteNextTask(dev1.GetId()) // goroutine 1: Arjun completes tid1
	}()

	wg.Add(1)
	go func() {
		defer wg.Done()
		taskApp.CompleteNextTask(dev3.GetId()) // goroutine 2: Rohan completes tid5
	}()

	wg.Add(1)
	go func() {
		defer wg.Done()
		taskApp.CompleteNextTask(dev2.GetId()) // goroutine 3: Sneha completes tid3
		taskApp.CompleteNextTask(dev2.GetId()) // goroutine 3: Sneha completes tid4
	}()

	wg.Wait() // wait for all goroutines to finish

	// ── 7. Concurrent task creation (stress test for atomic ID) ─────────────
	fmt.Println("\n--- Concurrent Task Creation (atomic ID test) ---")
	var wg2 sync.WaitGroup
	for i := 0; i < 5; i++ {
		wg2.Add(1)
		go func(i int) {
			defer wg2.Done()
			id := taskApp.CreateTask(admin.GetId(),
				fmt.Sprintf("Concurrent Task %d", i),
				"Created concurrently",
				5, enum.MEDIUM,
			)
			fmt.Printf("  Created: %s\n", id)
		}(i)
	}
	wg2.Wait()

	// ── 8. Search ───────────────────────────────────────────────────────────
	taskApp.SearchByPriority()
	taskApp.SearchByDueDate()

	// ── 9. History ──────────────────────────────────────────────────────────
	fmt.Println("\n--- Task Histories ---")
	taskApp.ShowHistory(dev1.GetId())
	taskApp.ShowHistory(dev2.GetId())
	taskApp.ShowHistory(dev3.GetId())

	// ── 10. Delete ──────────────────────────────────────────────────────────
	fmt.Println("\n--- Deleting Tasks ---")
	taskApp.DeleteTask(tid1)
	taskApp.DeleteTask("task999")

	fmt.Println("\n╔══════════════════════════════════════════╗")
	fmt.Println("║              Demo Complete               ║")
	fmt.Println("╚══════════════════════════════════════════╝")
	fmt.Println("\nTip: run with 'go run -race ./...' to verify no data races.")
}