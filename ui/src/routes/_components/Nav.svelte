<script>
	import { stores } from "@sapper/app";

	const { page, session } = stores();
</script>

<nav class="navbar navbar-expand-lg navbar-light bg-light">
	<div class="container-fluid">
		<a class="navbar-brand" href=".">Arth</a>
		<button
			class="navbar-toggler"
			type="button"
			data-bs-toggle="collapse"
			data-bs-target="#navbarSupportedContent"
			aria-controls="navbarSupportedContent"
			aria-expanded="false"
			aria-label="Toggle navigation">
			<span class="navbar-toggler-icon" />
		</button>
		<div
			class="collapse navbar-collapse"
			id="navbarSupportedContent"
			style="float:right">
			<ul class="navbar-nav me-auto mb-2 mb-lg-0">
				<li class="nav-item">
					<a
						rel="prefetch"
						class="nav-link"
						class:active={$page.path === '/'}
						href="/">Home</a>
				</li>

				{#if $session.user}
					<li class="nav-item">
						<a
							rel="prefetch"
							href="/editor"
							class="nav-link"
							class:active={$page.path === '/editor'}>
							<i class="ion-compose" />&nbsp;New Post
						</a>
					</li>

					<li class="nav-item">
						<a
							rel="prefetch"
							href="/settings"
							class="nav-link"
							class:active={$page.path === '/settings'}>
							<i class="ion-gear-a" />&nbsp;Settings
						</a>
					</li>

					<li class="nav-item">
						<a
							rel="prefetch"
							href="/profile/@{$session.user.username}"
							class="nav-link">
							<!-- <img src={$user.image} class="user-pic" alt={$user.username}> -->
							{$session.user.username}
						</a>
					</li>
				{:else}
					<li class="nav-item">
						<a
							rel="prefetch"
							href="/login"
							class="nav-link"
							class:active={$page.path === '/login'}>
							Sign in
						</a>
					</li>

					<li class="nav-item">
						<a
							rel="prefetch"
							href="/register"
							class="nav-link"
							class:active={$page.path === '/register'}>
							Register
						</a>
					</li>
				{/if}
			</ul>
			<form class="d-flex">
				<input
					class="form-control me-2"
					type="search"
					placeholder="Search"
					aria-label="Search" />
				<button
					class="btn btn-outline-success"
					type="submit">Search</button>
			</form>
		</div>
	</div>
</nav>
