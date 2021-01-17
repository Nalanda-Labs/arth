<script>
	import { goto, stores } from "@sapper/app";
	import { post } from "utils.js";

	const { page, session } = stores();

	async function logout() {
		await post(`auth/logout`);
		$session.user = null;
		localStorage.removeItem("jwt");
		goto("/");
	}
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
				{#if $session.user}
					<li class="nav-item">
						<a
							rel="prefetch"
							href="/topic"
							class="nav-link"
							class:active={$page.path === '/topic'}>
							<i class="ion-compose" />&nbsp;Create Topic
						</a>
					</li>

					<li class="nav-item">
						<a
							rel="prefetch"
							href="/profile/{$session.user}"
							class="nav-link">
							<!-- <img src={$user.image} class="user-pic" alt={$user.username}> -->
							{$session.user}
						</a>
					</li>
					<li class="nav-item">
						<a
							rel="prefetch"
							href="logout"
							class="nav-link"
							on:click={logout}>
							Sign Out
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
				<button class="btn btn-primary" type="submit">Search</button>
			</form>
		</div>
	</div>
</nav>
